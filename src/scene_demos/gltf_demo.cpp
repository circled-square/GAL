#include "gltf_demo.hpp"
#include <glm/glm.hpp>
#include <scluk/format.hpp>
#include <scluk/read_file.hpp>
#include <list>
#include <stdexcept>
#include <imgui.h>

using namespace std;
using namespace glm;
using namespace gl;
using namespace scluk;

namespace scene_demos {
    static tinygltf::Model load_gltf(const char* path, bool binary) {
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

        return meshes;
    }


    static tuple<vector<vertex_buffer>, vector<index_buffer>> make_buffers(tinygltf::Model& model) {
        vector<vertex_buffer> vbos;
        vector<index_buffer> ibos;

        for(size_t i = 0; i < model.bufferViews.size(); i++) {
            const tinygltf::BufferView &bufView = model.bufferViews[i];
            const tinygltf::Buffer &buf = model.buffers[bufView.buffer];

            //out("Buffer& buf = model.buffers[%];", bufView.buffer);

            if (bufView.target == TINYGLTF_TARGET_ARRAY_BUFFER) {
                vbos.emplace_back(&buf.data[bufView.byteOffset], bufView.byteLength, bufView.byteStride);
                out("vbos.emplace_back(&buf.data[%], %, %);", bufView.byteOffset, bufView.byteLength, bufView.byteStride);
            } else {
                ibos.emplace_back(reinterpret_cast<const uvec3*>(&buf.data[bufView.byteOffset]),bufView.byteLength);
                out("ibos.emplace_back(&buf.data[%], %);", bufView.byteOffset, bufView.byteLength);
                assert(bufView.byteStride == sizeof(uvec3) || bufView.byteStride == 0);
            }
        }

        return { move(vbos), move(ibos) };
    }

    static vertex_layout get_meshes_vertex_layout(tinygltf::Model& model, vector<tinygltf::Mesh*>& meshes) {
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
                        attrib_name == "NORMAL" ? 1 :
                        attrib_name == "TEXCOORD_0" ? 2 : -1;

                    if(vaa > -1) {
                        vertex_array_attrib attrib(vaa, accessor.byteOffset, accessor.componentType, size, accessor.bufferView, accessor.normalized);
                        vaas[vaa] = attrib;
                    } else
                        out("vaa missing: %", attrib_name);
                }
            }
        }

        vertex_layout layout;
        for(auto&[index, attrib] : vaas)
            layout.attribs.push_back(attrib);

        return layout;
    }

    texture get_model_texture(tinygltf::Model& model) {
        assert(model.textures.size() > 0);
        // fixme: Use material's baseColor
        tinygltf::Texture& tex = model.textures[0];

        assert(tex.source > -1);

        tinygltf::Image& image = model.images[tex.source];

        assert(image.bits == 8);

        return texture(image.image.data(), image.width, image.height, image.component, 1);
    }

    vertex_array make_model_vao(tinygltf::Model& model) {
        auto[vbos, ibos] = make_buffers(model);

        vector<tinygltf::Mesh*> meshes = get_model_meshes(model);
        assert(meshes.size() == model.meshes.size());
        
        vertex_layout vertex_layout = get_meshes_vertex_layout(model, meshes);

        return vertex_array(move(vbos), move(ibos), move(vertex_layout));
    }

    model_t gltf_demo::make_model() {
        tinygltf::Model model = load_gltf("src/third_party/tinygltf/models/Cube/Cube.gltf", false);

        vertex_array vao = make_model_vao(model);
        texture tex = get_model_texture(model);
        shader_program shader(read_file("src/shader/gltf_demo/vert.glsl"), read_file("src/shader/gltf_demo/frag.glsl"));

        return model_t { move(vao), move(tex), move(shader) };
    }

    gltf_demo::gltf_demo() : m_renderer(), m_model(make_model()) {}

    void gltf_demo::update(float delta) {
        m_model.shader.set_uniform("u_mvp", mat4(1));
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
