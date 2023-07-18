#include "gltf_demo.hpp"
#include <glm/glm.hpp>
#include <scluk/format.hpp>
#include <scluk/read_file.hpp>
#include <list>
#include <stdexcept>
#include <imgui.h>

using namespace glm;
using namespace scluk;

//TODO: populate IBOs

namespace scene_demos {
    static tinygltf::Model load_gltf(const char* path, bool binary) {
        tinygltf::TinyGLTF loader;
        tinygltf::Model model;
        std::string err, warn;

        bool success;
        if(binary) {
            success = loader.LoadBinaryFromFile(&model, &err, &warn, path); // for .glb
        } else {
            success = loader.LoadASCIIFromFile(&model, &err, &warn, path); // for .gltf
        }

        if(!warn.empty())
          out("Warn: %", warn.c_str());
        
        if(!err.empty())
          out("Err: %", err.c_str());
        
        if(!success)
            throw std::runtime_error("Fatal error: Failed to parse glTF");
        
        return model;
    }
    static std::vector<tinygltf::Mesh*> get_model_meshes(tinygltf::Model& model) {
        std::vector<tinygltf::Mesh*> meshes;

        const tinygltf::Scene &scene = model.scenes[model.defaultScene];
        std::list<tinygltf::Node*> nodes;

        for(size_t i = 0; i < scene.nodes.size(); i++) {
            assert(scene.nodes[i] >= 0 && scene.nodes[i] < model.nodes.size());
            nodes.push_back(&model.nodes[scene.nodes[i]]);
        }

        while(!nodes.empty()) {
            tinygltf::Node& node = *nodes.front();
            nodes.pop_front();

            if((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
                meshes.push_back(&model.meshes[node.mesh]);
            }

            for(size_t i = 0; i < node.children.size(); i++) {
                assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
                nodes.push_back(&model.nodes[node.children[i]]);
            }
        }

        return meshes;
    }
    static std::vector<gl::vertex_buffer> make_vbos(tinygltf::Model& model) {
        std::vector<gl::vertex_buffer> vbos;

	    for(size_t i = 0; i < model.bufferViews.size(); i++) {
            const tinygltf::BufferView& bufferView = model.bufferViews[i];
            const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

            vbos.emplace_back(&buffer.data[bufferView.byteOffset], bufferView.byteLength, bufferView.byteStride);
            //for now we shall simply ignore bufferView.target
	    }

        return vbos;
    }

    static gl::vertex_layout get_meshes_vertex_layout(tinygltf::Model& model, std::vector<tinygltf::Mesh*>& meshes) {
        using vertex_array_attrib = gl::vertex_layout::vertex_array_attrib;
        std::unordered_map<int, vertex_array_attrib> vaas; // avoid repeating attribs in the layout.attribs vector

        for(tinygltf::Mesh* mesh_p : meshes) {
            tinygltf::Mesh& mesh = *mesh_p;

            for(size_t i = 0; i < mesh.primitives.size(); ++i) {
                tinygltf::Primitive primitive = mesh.primitives[i];
                //tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

                for(auto& attrib : primitive.attributes) {
                    tinygltf::Accessor accessor = model.accessors[attrib.second];
                    int byteStride = accessor.ByteStride(model.bufferViews[accessor.bufferView]);

                    int size = accessor.type == TINYGLTF_TYPE_SCALAR ? 1 : accessor.type;

                    //vertex array attribute
                    int vaa = 
                        attrib.first.compare("POSITION") == 0 ? 0 :
                        attrib.first.compare("NORMAL") == 0 ? 1 :
                        attrib.first.compare("TEXCOORD_0") == 0 ? 2 : -1;

                    if(vaa > -1) {
                        vertex_array_attrib attrib(vaa, accessor.byteOffset, accessor.componentType, size, accessor.bufferView, accessor.normalized);
                        vaas[vaa] = attrib;
                    } else
                        out("vaa missing: %", attrib.first);
                }
            }
        }

        gl::vertex_layout layout;
        for(auto&[index, attrib] : vaas)
            layout.attribs.push_back(attrib);

        return layout;
    }
    gl::texture get_model_texture(tinygltf::Model& model) {
        assert(model.textures.size() > 0);
        // fixme: Use material's baseColor
        tinygltf::Texture& tex = model.textures[0];

        assert(tex.source > -1);

        tinygltf::Image& image = model.images[tex.source];

        assert(image.bits == 8);

        return gl::texture(image.image.data(), image.width, image.height, image.component, 1);
    }

    gl::vertex_array make_model_vao(tinygltf::Model& model) {
        std::vector<gl::vertex_buffer> vbos = make_vbos(model);
        std::vector<gl::index_buffer> ibos; ///TODO

        std::vector<tinygltf::Mesh*> meshes = get_model_meshes(model);
        assert(meshes.size() == model.meshes.size());
        
        gl::vertex_layout vertex_layout = get_meshes_vertex_layout(model, meshes);

        return gl::vertex_array(std::move(vbos), std::move(ibos), std::move(vertex_layout));
    }

    model_t gltf_demo::make_model() {
        tinygltf::Model model = load_gltf("src/third_party/tinygltf/models/Cube/Cube.gltf", false);

        gl::vertex_array vao = make_model_vao(model);
        gl::texture tex = get_model_texture(model);
        using scluk::read_file;
        gl::shader_program shader(read_file("src/shader/gltf_demo/vert.glsl"), read_file("src/shader/gltf_demo/frag.glsl"));

        return model_t { std::move(vao), std::move(tex), std::move(shader) };
    }

    gltf_demo::gltf_demo() : m_renderer(), m_model(make_model()) {}

    void gltf_demo::update(float delta) {
        m_model.shader.set_uniform("u_mvp", glm::mat4(1));
    }

    void gltf_demo::render() {
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
        m_model.texture.bind(0);
        m_model.shader.set_uniform("u_texture_slot", 0);
    }

} // scene_demos
