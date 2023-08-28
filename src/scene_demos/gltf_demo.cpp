#include "gltf_demo.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <list>
#include <stdexcept>
#include <imgui.h>
#include <scluk/format.hpp>
#include <scluk/read_file.hpp>

template<int size, typename T, glm::qualifier q>
std::string vec_to_string(glm::vec<size, T, q> v) {
    std::ostringstream os;
    os << "(";
    for(int i = 0; i < size-1; i++)
        os << v[i] << ", ";
    os << v[size-1] << ")";

    return os.str();
}

std::ostream& operator<<(std::ostream& os, glm::uvec3 v) { return os << vec_to_string(v); }
std::ostream& operator<<(std::ostream& os, glm::u16vec3 v) { return os << vec_to_string(v); }


using namespace std;
using namespace glm;
using namespace gl;
using namespace scluk;


namespace scene_demos {
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
            out("Warn: %", warn.c_str());
        
        if(!err.empty())
            out("Err: %", err.c_str());
        
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
            //out("nodes.push_back(&model.nodes[%]); (scene.nodes[%])", scene.nodes[i], i);
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
                //out("nodes.push_back(&model.nodes[%]); (node.children[%])", node.children[i], i);
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


                //TODO: these prints need to go (they are also specific to a precise gltf file)
                /*if(vbos.size() == 0) { //"POSITION" vbo -> print contents
                    const vec3* begin = reinterpret_cast<const vec3*>(&buf.data[bufView.byteOffset]);
                    const vec3* end = reinterpret_cast<const vec3*>(&buf.data[bufView.byteOffset+bufView.byteLength]);
                    for(const vec3* p = begin; p < end; p++) {
                        out("{ %, %, % },", p->x, p->y, p->z);
                    }
                }
                if(vbos.size() == 3) { //"TEXCOORD_0" vbo -> print contents
					const vec2* begin = reinterpret_cast<const vec2*>(&buf.data[bufView.byteOffset]);
					const vec2* end = reinterpret_cast<const vec2*>(&buf.data[bufView.byteOffset+bufView.byteLength]);
					out("end - begin = %", end - begin);
					for(const vec2* p = begin; p < end; p++) {
						out("{ %, % },", p->x, p->y);
					}
				}*/

                vbos.emplace_back(&buf.data[bufView.byteOffset], bufView.byteLength, bufView.byteStride);
                out("vbos.emplace(&buf.data[%], %, %);", bufView.byteOffset, bufView.byteLength, bufView.byteStride);
            } else {
                const int element_typeid = get_element_typeid_of_bufview(model, i);

                const size_t size = bufView.byteLength / typeid_to_size(element_typeid);
                //TODO: uncomment this instead of the following code block
                const size_t tri_count = size/3;

                /*{
					const u16vec3* begin = reinterpret_cast<const u16vec3*>(&buf.data[bufView.byteOffset]);
					const u16vec3* end = reinterpret_cast<const u16vec3*>(&buf.data[bufView.byteOffset+bufView.byteLength]);
					out("end - begin = %", end - begin);
					for(const u16vec3* p = begin; p < end; p++) {
						out("{ %, %, % },", p->x, p->y, p->z);
					}
                }*/

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
                        attrib_name == "TEXCOORD_0" ? 1 : -1;
                        //TODO: uncomment this
                        //attrib_name == "NORMAL" ? 2 : -1;

                    if(vaa != -1) {
                        int vbo_bind = bufview_to_vbo_map[accessor.bufferView];
                        vertex_array_attrib attrib(vaa, accessor.byteOffset, accessor.componentType, size, vbo_bind, accessor.normalized);
                        out("%: (index=%, offset=%, type=%, size=%, bind_index=%, normalized=%)",
                                attrib_name, vaa, accessor.byteOffset, accessor.componentType, size, vbo_bind, accessor.normalized);
                        vaas[vaa] = attrib;
                    } else {
                        //out("%: (index=MISSING, offset=%, type=%, size=%, bind_index=%, normalized=%)",
                        //    attrib_name, accessor.byteOffset, accessor.componentType, size, bufview_to_vbo_map[accessor.bufferView], accessor.normalized);
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
        // fixme: Use material's baseColor
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

    static vertex_array make_model_vao(tinygltf::Model& model) {
        auto[vbos, ibos, bufview_to_vbo_map] = make_buffers(model);

        vector<tinygltf::Mesh*> meshes = get_model_meshes(model);
        
        vertex_layout vertex_layout = get_meshes_vertex_layout(model, meshes, bufview_to_vbo_map);

        return vertex_array(move(vbos), move(ibos), move(vertex_layout));
    }

    static vertex_array make_dbg_vao() {
    	static constexpr std::array<vec3, 36> __positions {
    		vec3
    		{  1, -1,  1 }, { -1, -1, -1 }, {  1, -1, -1 },
			{ -1,  1, -1 }, {  1,  1,  1 }, {  1,  1, -1 },
    		{  1,  1, -1 }, {  1, -1,  1 }, {  1, -1, -1 },
    		{  1,  1,  1 }, { -1, -1,  1 }, {  1, -1,  1 },
    		{ -1, -1,  1 }, { -1,  1, -1 }, { -1, -1, -1 },
    		{  1, -1, -1 }, { -1,  1, -1 }, {  1,  1, -1 },
    		{  1, -1,  1 }, { -1, -1,  1 }, { -1, -1, -1 },
    		{ -1,  1, -1 }, { -1,  1,  1 }, {  1,  1,  1 },
    		{  1,  1, -1 }, {  1,  1,  1 }, {  1, -1,  1 },
    		{  1,  1,  1 }, { -1,  1,  1 }, { -1, -1,  1 },
    		{ -1, -1,  1 }, { -1,  1,  1 }, { -1,  1, -1 },
    		{  1, -1, -1 }, { -1, -1, -1 }, { -1,  1, -1 },
    	};
    	static constexpr std::array<vec2, 36> __tex_coords {
    		vec2
			{  0,  0 }, { -1,  1 }, {  0,  1 },
			{  0,  0 }, {  1, -1 }, {  1, -0 },
			{  1,  0 }, {  0, -1 }, {  1, -1 },
			{  1,  0 }, { -0, -1 }, {  1, -1 },
			{  0,  0 }, {  1,  1 }, {  1,  0 },
			{  0,  0 }, { -1,  1 }, {  0,  1 },
			{  0,  0 }, { -1,  0 }, { -1,  1 },
			{  0,  0 }, { -0, -1 }, {  1, -1 },
			{  1,  0 }, { -0,  0 }, {  0, -1 },
			{  1,  0 }, { -0,  0 }, { -0, -1 },
			{  0,  0 }, {  0,  1 }, {  1,  1 },
			{  0,  0 }, { -1,  0 }, { -1,  1 },
    	};
    	static constexpr std::array<u16vec3, 12> __indices {
    		u16vec3
    		{  0,  1,  2 },
    		{  3,  4,  5 },
    		{  6,  7,  8 },
    		{  9, 10, 11 },
    		{ 12, 13, 14 },
    		{ 15, 16, 17 },
    		{ 18, 19, 20 },
    		{ 21, 22, 23 },
    		{ 24, 25, 26 },
    		{ 27, 28, 29 },
    		{ 30, 31, 32 },
    		{ 33, 34, 35 },
    	};

    	static constexpr std::array<int, 20> junk_data {
    		3, 7, 3, 7, 8, 3, 7, 3, 7, 8, 3, 7, 3, 7, 8, 3, 7, 3, 7, 8,
    	};

        vertex_layout vertex_layout;
        vertex_layout::vertex_array_attrib attrib;
        attrib.index = 0;
        attrib.offset = 0;
        attrib.type_id = GL_FLOAT;
        attrib.size = 3;
        attrib.vao_vbo_bind_index = 0;
        vertex_layout.attribs.push_back(attrib);

        attrib.index = 1;
        attrib.offset = 0;
        attrib.type_id = GL_FLOAT;
        attrib.size = 2;
        attrib.vao_vbo_bind_index = 3;
        vertex_layout.attribs.push_back(attrib);


        vector<vertex_buffer> vbos;
        vbos.reserve(2);
        vbos.push_back(vertex_buffer(__positions));
        vbos.push_back(vertex_buffer(junk_data));
        vbos.push_back(vertex_buffer(junk_data));
        vbos.push_back(vertex_buffer(__tex_coords));

        vector<index_buffer> ibos;
        ibos.reserve(1);
        ibos.push_back(index_buffer(__indices));

        return vertex_array(move(vbos), move(ibos), move(vertex_layout));
    }

    static model_t make_model() {
        tinygltf::Model model = load_gltf_from_file("src/third_party/tinygltf/models/Cube/Cube.gltf", false);

        //TODO: uncomment this
        //vertex_array vao = make_model_vao(model);
        vertex_array vao = make_dbg_vao();

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
