#include "gltf_demo.hpp"

#include <tinygltf/tiny_gltf.h>
#include <glm/gtc/matrix_transform.hpp>
#include <list>
#include <unordered_set>
#include <stdexcept>
#include <imgui.h>
#include <scluk/format.hpp>
#include <scluk/read_file.hpp>

template<int size, typename T, glm::qualifier q>
std::ostream& fmt_vec_to_stream(std::ostream& os, glm::vec<size, T, q> v) {
    os << "(";
    for(int i = 0; i < size-1; i++)
        os << v[i] << ", ";
    os << v[size-1] << ")";

    return os;
}

std::ostream& operator<<(std::ostream& os, glm::uvec3 v) { return fmt_vec_to_stream(os, v); }
std::ostream& operator<<(std::ostream& os, glm::u16vec3 v) { return fmt_vec_to_stream(os, v); }




namespace scene_demos {
    using namespace std;
    using namespace glm;
    using namespace gl;
    using namespace scluk;

    constexpr vec3 x_axis = vec3(1,0,0), y_axis = vec3(0,1,0), z_axis = vec3(0,0,1);
    constexpr float pi = glm::pi<f32>();

    static mat4 make_model_matrix(vec3 pos, float rotation, float scale_factor) {
        return scale(
            rotate(
                translate(
                    mat4(1),
                    pos
                ),
                rotation,
                y_axis
            ),
            vec3(scale_factor, scale_factor, scale_factor)
        );
    }

    static mat4 make_view_matrix(vec3 pos, float rotation) {
        return rotate(
            translate(
                    mat4(1.f),
                    -pos
            ),
            -rotation,
            z_axis
        );
    }

    static mat4 make_proj_matrix(vec2 aspect_ratio) {
        return perspective(pi / 4, aspect_ratio.x / aspect_ratio.y, .1f, 100.f);
    }

    static tinygltf::Model load_gltf_from_file(const char* path, bool binary) {
        tinygltf::TinyGLTF loader;
        tinygltf::Model model;
        string err, warn;

        bool success =
            binary ? loader.LoadBinaryFromFile(&model, &err, &warn, path) // for .glb
            : loader.LoadASCIIFromFile(&model, &err, &warn, path); // for .gltf

        if(!warn.empty())
            out("Warn: %", warn);
        
        if(!err.empty())
            out("Err: %", err);
        
        if(!success)
            throw runtime_error("Fatal error: Failed to parse glTF");
        
        return model;
    }

    static vector<tinygltf::Mesh*> get_model_meshes(tinygltf::Model& model) {

        //wtf does this function even do... why not just read the model.meshes array?

        vector<tinygltf::Mesh*> meshes;

        const tinygltf::Scene &scene = model.scenes[model.defaultScene];
        list<tinygltf::Node*> nodes;

        for(size_t i = 0; i < scene.nodes.size(); i++) {
            assert(scene.nodes[i] >= 0 && scene.nodes[i] < model.nodes.size());
            nodes.push_back(&model.nodes[scene.nodes[i]]);
        }

        while(!nodes.empty()) {
            tinygltf::Node& node = *nodes.front();
            nodes.pop_front();

            if((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
                meshes.push_back(&model.meshes[node.mesh]);
                out("mesh.push_back(&model.meshes[%]);", node.mesh);
            }

            for(size_t i = 0; i < node.children.size(); i++) {
                assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
                nodes.push_back(&model.nodes[node.children[i]]);
            }
        }

        assert(meshes.size() == model.meshes.size());

        return meshes;
    }

    static int get_element_typeid_of_bufview(tinygltf::Model& model, int bufview) {
        int element_typeid = -1;
        for(tinygltf::Accessor& a : model.accessors) {
            if(a.bufferView == bufview) {
                element_typeid = a.componentType;
                break;
            }
        }
        assert(element_typeid != -1);

        return element_typeid;
    }

    using bufview_to_vbo_map_t = vector<int>;

    static tuple<vector<vertex_buffer>, vector<index_buffer>, bufview_to_vbo_map_t> make_buffers(tinygltf::Model& model) {
        vector<vertex_buffer> vbos;
        vector<index_buffer> ibos;
        bufview_to_vbo_map_t bufview_to_vbo_map(model.bufferViews.size(), -1);

        for(size_t i = 0; i < model.bufferViews.size(); i++) {
            const tinygltf::BufferView &bufView = model.bufferViews[i];
            const tinygltf::Buffer &buf = model.buffers[bufView.buffer];

            if (bufView.target == TINYGLTF_TARGET_ARRAY_BUFFER) {
                bufview_to_vbo_map[i] = vbos.size();

                vbos.emplace_back(&buf.data[bufView.byteOffset], bufView.byteLength, bufView.byteStride);
                out("vbos.emplace(&buf.data[%], %, %);", bufView.byteOffset, bufView.byteLength, bufView.byteStride);
            } else {
                const int element_typeid = get_element_typeid_of_bufview(model, i);

                const size_t size = bufView.byteLength / typeid_to_size(element_typeid);
                const size_t tri_count = size/3;

                ibos.push_back(index_buffer(buffer(&buf.data[bufView.byteOffset], bufView.byteLength), tri_count, element_typeid));

                out("ibos.emplace(&buf.data[%], %, type=u%); stride=% tri_count=%",
                    bufView.byteOffset, bufView.byteLength, 8*typeid_to_size(element_typeid), bufView.byteStride,tri_count);

                assert(bufView.byteStride == 0 || typeid_to_size(element_typeid) == bufView.byteStride);
            }
        }

        return { move(vbos), move(ibos), move(bufview_to_vbo_map) };
    }

    static vertex_layout get_meshes_vertex_layout(tinygltf::Model& model, vector<tinygltf::Mesh*>& meshes, const bufview_to_vbo_map_t& bufview_to_vbo_map) {
        using vertex_array_attrib = vertex_layout::vertex_array_attrib;
        unordered_map<int, vertex_array_attrib> vaas; // avoid repeating attribs in the layout.attribs vector

        for(tinygltf::Mesh* mesh_p : meshes) {
            tinygltf::Mesh& mesh = *mesh_p;

            for(size_t i = 0; i < mesh.primitives.size(); ++i) {
                tinygltf::Primitive primitive = mesh.primitives[i];
                //tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

                for(auto& [attrib_name, attrib_accessor] : primitive.attributes) {
                    tinygltf::Accessor accessor = model.accessors[attrib_accessor];
                    //int byteStride = accessor.ByteStride(model.bufferViews[accessor.bufferView]);

                    int size = accessor.type == TINYGLTF_TYPE_SCALAR ?  1 : accessor.type;

                    //vertex array attribute
                    int vaa = 
                        attrib_name == "POSITION" ? 0 :
                        attrib_name == "TEXCOORD_0" ? 1 :
                        attrib_name == "NORMAL" ? 2 : -1;

                    if(vaa != -1) {
                        int vbo_bind = bufview_to_vbo_map[accessor.bufferView];
                        vertex_array_attrib attrib(vaa, accessor.byteOffset, accessor.componentType, size, vbo_bind, accessor.normalized);
                        out("%: (index=%, offset=%, type=%, size=%, bind_index=%, normalized=%)",
                                attrib_name, vaa, accessor.byteOffset, accessor.componentType, size, vbo_bind, accessor.normalized);
                        vaas[vaa] = attrib;
                    } else {
                        out("%: (index=MISSING, offset=%, type=%, size=%, bind_index=%, normalized=%)",
                            attrib_name, accessor.byteOffset, accessor.componentType, size, bufview_to_vbo_map[accessor.bufferView], accessor.normalized);
                    }
                }
            }
        }

        vertex_layout layout;
        for(auto&[index, attrib] : vaas)
            layout.attribs.push_back(attrib);

        return layout;
    }

    static texture get_model_texture(tinygltf::Model& model) {
        assert(model.textures.size() > 0);
        tinygltf::Texture& tex = model.textures[0];

        assert(tex.source > -1);

        tinygltf::Image& image = model.images[tex.source];

        assert(image.bits == 8);

        texture::specification spec;
        spec.w = image.width;
        spec.h = image.height;
        spec.components = 4;
        spec.data = image.image.data();
        spec.alignment = 1;
        spec.repeat_wrap = true;
        return texture(spec);
    }

    static void deduce_vbo_strides(vector<vertex_buffer>& vbos, const vertex_layout& vertex_layout) {
        unordered_set<size_t> vbos_to_deduce_stride_of;
        for(size_t i = 0; i < vbos.size(); i++) {
            if(vbos[i].get_stride() == 0)
                vbos_to_deduce_stride_of.insert(i);
        }

        for(const auto& attrib : vertex_layout.attribs) {
            size_t vbo_index = attrib.vao_vbo_bind_index;
            if(vbos_to_deduce_stride_of.contains(vbo_index)) {
                size_t attrib_size = attrib.size * typeid_to_size(attrib.type_id);
                vbos[vbo_index].set_stride(vbos[vbo_index].get_stride() + attrib_size);
            }
        }
    }

    static vertex_array make_model_vao(tinygltf::Model& model) {
        auto[vbos, ibos, bufview_to_vbo_map] = make_buffers(model);

        vector<tinygltf::Mesh*> meshes = get_model_meshes(model);
        
        vertex_layout vertex_layout = get_meshes_vertex_layout(model, meshes, bufview_to_vbo_map);

        deduce_vbo_strides(vbos, vertex_layout);

        return vertex_array(move(vbos), move(ibos), move(vertex_layout));
    }

    static model_t make_model() {
        tinygltf::Model model = load_gltf_from_file("src/third_party/tinygltf/models/Cube/Cube.gltf", false);

        vertex_array vao = make_model_vao(model);
        texture tex = get_model_texture(model);
        shader_program shader(read_file("src/shader/gltf_demo/vert.glsl"), read_file("src/shader/gltf_demo/frag.glsl"));

        return model_t { move(vao), move(tex), move(shader) };
    }


    gltf_demo::gltf_demo()
        : m_renderer(),
          m_model(make_model()),
          //compute the mvp matrix
          m_model_mat(make_model_matrix(vec3(0,0 ,-1), pi/2, .125)),
          m_view_mat(make_view_matrix(vec3(0,0,0), -pi/2)),
          m_proj_mat(make_proj_matrix(vec2(4,3))),
          m_mvp(m_proj_mat * m_view_mat * m_model_mat)
    {}

    void gltf_demo::update(float delta) {
        // edit the MVPs...
        m_model_mat = rotate(m_model_mat,  delta * pi / 16, y_axis);
        m_model_mat = rotate(m_model_mat,  delta * pi / 8, z_axis);

        m_mvp = m_proj_mat * m_view_mat * m_model_mat;
    }

    void gltf_demo::render() {
        m_model.shader.set_uniform("u_mvp", m_mvp);
        m_renderer.clear();

        for(size_t i = 0; i < m_model.vao.get_ibo_count(); i++) {
            m_renderer.draw(m_model.vao, m_model.shader, i);
        }

        {
            ImGui::Begin("gltf demo");
            ImGui::Text("Application average %.1f FPS", ImGui::GetIO().Framerate);
            ImGui::End();
        }

        scene_with_previous_scene::render();
    }
 
    void gltf_demo::reheat() {
        m_renderer.set_clear_color(vec4(0,0,0,1));
        const int texture_slot = 0;
        m_model.texture.bind(texture_slot);
        m_model.shader.set_uniform("u_texture_slot", texture_slot);
        m_model.shader.set_uniform("u_mvp", m_mvp);


        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendEquation(GL_FUNC_ADD);
        glEnable(GL_BLEND);

        // Enable depth test
        glEnable(GL_DEPTH_TEST);
        // Accept fragment if it closer to the camera than the former one
        glDepthFunc(GL_LESS);
    }

} // scene_demos
