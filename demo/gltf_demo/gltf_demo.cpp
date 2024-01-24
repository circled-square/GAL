#include "gltf_demo.hpp"

#include <tinygltf/tiny_gltf.h>
#include <glm/gtc/matrix_transform.hpp>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include <stdexcept>
#include <imgui.h>
#include <scluk/log.hpp>
#include <scluk/read_file.hpp>
#include <scluk/aliases.hpp>

namespace scene_demos {
    using namespace std;
    using namespace glm;
    using namespace scluk;

    constexpr vec3 x_axis = vec3(1,0,0), y_axis = vec3(0,1,0), z_axis = vec3(0,0,1);
    constexpr float pi = glm::pi<f32>();

    static mat4 make_model_matrix(vec3 pos, float rotation) {
        return rotate(
            translate(mat4(1), pos),
            rotation,
            z_axis
        );
    }
    static mat4 make_view_matrix(vec3 pos, float rotation) {
        return rotate(
            translate(mat4(1), -pos),
            -rotation,
            z_axis
        );
    }
    static mat4 make_proj_matrix(vec2 aspect_ratio) {
        return perspective(pi / 4, aspect_ratio.x / aspect_ratio.y, .1f, 100.f);
    }
    static mvp_t make_mvp() {
        mat4 model = make_model_matrix(vec3(0, 0, 0), 0);
        mat4 view = make_view_matrix(vec3(0, 1.0, 4.0), 0);
        mat4 proj = make_proj_matrix(vec2(16, 9));

        return mvp_t {
            .model = model,
            .view = view,
            .proj = proj,
            .mvp = proj * view * model
        };
    }

    static tinygltf::Model load_gltf_from_file(const char* path, bool binary) {
        tinygltf::TinyGLTF loader;
        tinygltf::Model model;
        string err, warn;

        bool success =
            binary ? loader.LoadBinaryFromFile(&model, &err, &warn, path) // for .glb
            : loader.LoadASCIIFromFile(&model, &err, &warn, path); // for .gltf

        if(!warn.empty())
            stdout_log("Warn: %", warn);
        
        if(!err.empty())
            stdout_log("Err: %", err);
        
        if(!success)
            throw runtime_error("Fatal error: Failed to parse glTF");
        
        return model;
    }

    using bufview_to_vbo_map_t = vector<i32>;

    static tuple<vector<gal::graphics::vertex_buffer>, vector<gal::graphics::index_buffer>, bufview_to_vbo_map_t> make_buffers(tinygltf::Model& model) {
        vector<gal::graphics::vertex_buffer> vbos;
        vector<gal::graphics::index_buffer> ibos;
        bufview_to_vbo_map_t bufview_to_vbo_map(model.bufferViews.size(), -1);

        bool contains_bufviews_without_target = false;

        vector<i32> bufview_to_accessor(model.bufferViews.size(), -1);
        for(size_t i = 0; i < model.accessors.size(); i++) {
            bufview_to_accessor[model.accessors[i].bufferView] =  i;
        }

        for(size_t i = 0; i < model.bufferViews.size(); i++) {
            assert(bufview_to_accessor[i] >= 0);

            const tinygltf::BufferView& bufview = model.bufferViews[i];
            const tinygltf::Buffer& buf = model.buffers[bufview.buffer];
            const tinygltf::Accessor& accessor = model.accessors[bufview_to_accessor[i]];

            if (bufview.target == TINYGLTF_TARGET_ARRAY_BUFFER) {
                bufview_to_vbo_map[i] = vbos.size();

                vbos.emplace_back(&buf.data[bufview.byteOffset], bufview.byteLength, bufview.byteStride);
            } else if (bufview.target == TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER) {
                const int element_typeid = accessor.componentType;

                const size_t size = bufview.byteLength / gal::graphics::typeid_to_size(element_typeid);
                const size_t tri_count = size/3;

                assert(bufview.byteStride == 0 || typeid_to_size(element_typeid)*3 == bufview.byteStride);

                ibos.emplace_back(gal::graphics::buffer(&buf.data[bufview.byteOffset], bufview.byteLength), tri_count, element_typeid);

            } else {
                contains_bufviews_without_target = true;
            }
        }

        if(contains_bufviews_without_target) {
            stdout_log("warning: the gltf model contains bufferViews without a target property");
        }

        return { std::move(vbos), std::move(ibos), std::move(bufview_to_vbo_map) };
    }

    static gal::graphics::vertex_layout get_vertex_layout(tinygltf::Model& model, const bufview_to_vbo_map_t& bufview_to_vbo_map) {
        using vertex_array_attrib = gal::graphics::vertex_layout::vertex_array_attrib;
        unordered_map<int, vertex_array_attrib> vaas; // avoid repeating attribs in the layout.attribs vector

        for(const tinygltf::Mesh& mesh : model.meshes) {
            for(size_t i = 0; i < mesh.primitives.size(); ++i) {
                tinygltf::Primitive primitive = mesh.primitives[i];

                for(auto& [attrib_name, attrib_accessor] : primitive.attributes) {
                    tinygltf::Accessor accessor = model.accessors[attrib_accessor];
                    //int byteStride = accessor.ByteStride(model.bufferViews[accessor.bufferView]);

                    int size = accessor.type == TINYGLTF_TYPE_SCALAR ?  1 : accessor.type;

                    //vertex array attribute
                    int vaa = 
                        attrib_name == "POSITION" ? 0 :
                        attrib_name == "TEXCOORD_0" ? 1 : -1;

                    if(vaa != -1) {
                        int vbo_bind = bufview_to_vbo_map[accessor.bufferView];
                        vertex_array_attrib attrib(vaa, accessor.byteOffset, accessor.componentType, size, vbo_bind, accessor.normalized);
                        stdout_log("%: (index=%, offset=%, type=%, size=%, bind_index=%, normalized=%)",
                            attrib_name, vaa, accessor.byteOffset, accessor.componentType, size, vbo_bind, accessor.normalized);
                        vaas[vaa] = attrib;
                    } else {
                        stdout_log("%: (index=MISSING, offset=%, type=%, size=%, bind_index=%, normalized=%)",
                            attrib_name, accessor.byteOffset, accessor.componentType, size, bufview_to_vbo_map[accessor.bufferView], accessor.normalized);
                    }
                }
            }
        }

        gal::graphics::vertex_layout layout;
        for(auto&[index, attrib] : vaas)
            layout.attribs.push_back(attrib);

        return layout;
    }

    static gal::graphics::texture get_model_texture(tinygltf::Model& model) {
        assert(model.textures.size() > 0);

        tinygltf::Texture& tex = model.textures[0];

        assert(tex.source >= 0 && model.images.size() > tex.source);

        tinygltf::Image& image = model.images[tex.source];

        assert(image.bits == 8);

        gal::graphics::texture::specification spec;
        spec.w = image.width;
        spec.h = image.height;
        spec.components = 4;
        spec.data = image.image.data();
        spec.alignment = 1;
        spec.repeat_wrap = true;
        return gal::graphics::texture(spec);
    }

    static void deduce_vbo_strides(vector<gal::graphics::vertex_buffer>& vbos, const gal::graphics::vertex_layout& vertex_layout) {
        unordered_set<size_t> vbos_to_deduce_stride_of;
        for(size_t i = 0; i < vbos.size(); i++) {
            if(vbos[i].get_stride() == 0)
                vbos_to_deduce_stride_of.insert(i);
        }

        for(const auto& attrib : vertex_layout.attribs) {
            size_t vbo_index = attrib.vao_vbo_bind_index;
            if(vbos_to_deduce_stride_of.contains(vbo_index)) {
                size_t attrib_size = attrib.size * gal::graphics::typeid_to_size(attrib.type_id);
                vbos[vbo_index].set_stride(vbos[vbo_index].get_stride() + attrib_size);
            }
        }
    }

    static gal::graphics::vertex_array make_model_vao(tinygltf::Model& model) {
        auto[vbos, ibos, bufview_to_vbo_map] = make_buffers(model);
        gal::graphics::vertex_layout vertex_layout = get_vertex_layout(model, bufview_to_vbo_map);

        deduce_vbo_strides(vbos, vertex_layout);

        return gal::graphics::vertex_array(std::move(vbos), std::move(ibos), std::move(vertex_layout));
    }

    static model_t make_model() {
        //tinygltf::Model model = load_gltf_from_file("resources/blender-gltf-attempt/dice.gltf", false);
        tinygltf::Model model = load_gltf_from_file("resources/blender-gltf-attempt/human.gltf", false);
        //tinygltf::Model model = load_gltf_from_file("src/third_party/tinygltf/models/Cube/Cube.gltf", false);
        //tinygltf::Model model = load_gltf_from_file("resources/glTF-Sample-Models/1.0/Avocado/glTF/Avocado.gltf", false);
        //tinygltf::Model model = load_gltf_from_file("resources/glTF-Sample-Models/1.0/Duck/glTF/Duck.gltf", false);

        gal::graphics::vertex_array vao = make_model_vao(model);
        gal::graphics::texture tex = get_model_texture(model);
        gal::graphics::shader_program shader(read_file("demo/shaders/gltf_demo/vert.glsl"), read_file("demo/shaders/gltf_demo/frag.glsl"));

        return model_t { std::move(vao), std::move(tex), std::move(shader) };
    }


    gltf_demo::gltf_demo()
        : m_renderer(),
          m_model(make_model()),
          m_mvp(make_mvp())
    {}

    void gltf_demo::update(float delta) {
        // edit the MVP
        m_mvp.model = rotate(m_mvp.model, delta * pi / 16, y_axis);
        m_mvp.mvp = m_mvp.proj * m_mvp.view * m_mvp.model;
    }

    void gltf_demo::render() {
        m_model.shader.set_uniform("u_mvp", m_mvp.mvp);
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
        m_model.shader.set_uniform("u_mvp", m_mvp.mvp);


        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendEquation(GL_FUNC_ADD);
        glEnable(GL_BLEND);

        // Enable depth test
        glEnable(GL_DEPTH_TEST);
        // Accept fragment if it closer to the camera than the former one
        glDepthFunc(GL_LESS);
    }

} // scene_demos
