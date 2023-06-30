#include "gltf_demo.hpp"
#include <glm/glm.hpp>
#include <scluk/format.hpp>
#include <array>
#include <imgui.h>

using namespace glm;
using namespace scluk;

namespace scene_demos {
    struct vertex_t {
        vec2 pos;
        vec2 tex_coord;

        using layout_t = decltype(gl::vertex_layout(pos, tex_coord));
    };

    gltf_demo::gltf_demo()
            : //m_vao(gl::vertex_array::make<vertex_t>(gl::vertex_buffer(vertex_data), gl::index_buffer(indices))),
              //m_shader(read_file("src/shader/gltf_demo/vert.glsl"), read_file("src/shader/gltf_demo/frag.glsl")),
              m_renderer(),
              m_model()
    {
        tinygltf::TinyGLTF loader;
        std::string err, warn;

        //bool success = loader.LoadASCIIFromFile(&m_model, &err, &warn, "src/resources/example.gltf"); //for ASCII glTF (.gltf)
        bool success = loader.LoadBinaryFromFile(&m_model, &err, &warn, "src/resources/example.glb"); // for binary glTF(.glb)
        if (!warn.empty())
          out("Warn: %", warn.c_str());
        
        if (!err.empty())
          out("Err: %", err.c_str());
        
        if (!success) {
            out("Fatal error: Failed to parse glTF");
	        return;
	    }

        std::vector<gl::vertex_array> vbos;

	    for(size_t i = 0; i < m_model.bufferViews.size(); i++) {
            const tinygltf::BufferView& bufferView = m_model.bufferViews[i];
            const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

            vbos.emplace_back(&buffer.data[bufferView.byteOffset], bufferView.byteLength, bufferView.byteStride);
            //for now we shall simply ignore bufferView.target

            std::cout << "bufferview.target " << bufferView.target << std::endl;
            std::cout << "buffer.data.size = " << buffer.data.size()
                      << ", bufferview.byteOffset = " << bufferView.byteOffset
                      << std::endl;

	    }

        for (size_t i = 0; i < mesh.primitives.size(); ++i) {
            tinygltf::Primitive primitive = mesh.primitives[i];
            tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

            for (auto& attrib : primitive.attributes) {
                tinygltf::Accessor accessor = model.accessors[attrib.second];
                int byteStride = accessor.ByteStride(model.bufferViews[accessor.bufferView]);
                glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);

                int size = accessor.type == TINYGLTF_TYPE_SCALAR ? 1 : accessor.type;

                //vertex array attribute
                int vaa = 
                    attrib.first.compare("POSITION") == 0 ? 0 :
                    attrib.first.compare("NORMAL") == 0 ? 1 :
                    attrib.first.compare("TEXCOORD_0") == 0 ? 2 : -1;

                if (vaa > -1) {
                    glEnableVertexAttribArray(vaa);
                    glVertexAttribPointer(vaa, size, accessor.componentType, accessor.normalized, byteStride, BUFFER_OFFSET(accessor.byteOffset));
                } else
                    std::cout << "vaa missing: " << attrib.first << std::endl;
            }

            if (model.textures.size() > 0) {
                // fixme: Use material's baseColor
                tinygltf::Texture &tex = model.textures[0];

                if (tex.source > -1) {

                    GLuint texid;
                    glGenTextures(1, &texid);

                    tinygltf::Image &image = model.images[tex.source];

                    glBindTexture(GL_TEXTURE_2D, texid);
                    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                    GLenum format = GL_RGBA;

                    if (image.component == 1) {
                        format = GL_RED;
                    } else if (image.component == 2) {
                        format = GL_RG;
                    } else if (image.component == 3) {
                        format = GL_RGB;
                    } else {
                        // ???
                    }

                    GLenum type = GL_UNSIGNED_BYTE;
                    if (image.bits == 8) {
                        // ok
                    } else if (image.bits == 16) {
                        type = GL_UNSIGNED_SHORT;
                    } else {
                        // ???
                    }

                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
                                             format, type, &image.image.at(0));
                }
            }
            /*
            if (model.textures.size() > 0) {
                // fixme: Use material's baseColor
                tinygltf::Texture &tex = model.textures[0];

                if (tex.source > -1) {

                    GLuint texid;
                    glGenTextures(1, &texid);

                    tinygltf::Image &image = model.images[tex.source];

                    glBindTexture(GL_TEXTURE_2D, texid);
                    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                    GLenum format = GL_RGBA;

                    if (image.component == 1) {
                        format = GL_RED;
                    } else if (image.component == 2) {
                        format = GL_RG;
                    } else if (image.component == 3) {
                        format = GL_RGB;
                    } else {
                        // ???
                    }

                    GLenum type = GL_UNSIGNED_BYTE;
                    if (image.bits == 8) {
                        // ok
                    } else if (image.bits == 16) {
                        type = GL_UNSIGNED_SHORT;
                    } else {
                        // ???
                    }

                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
                                             format, type, &image.image.at(0));
                }
            }
            */
        }
    }

    void gltf_demo::update(float delta) {}

    void gltf_demo::render() {
        m_renderer.clear();

        //m_shader.set_uniform("u_mvp", glm::mat4(1));
        //m_renderer.draw(m_vao, m_shader);

        {
            ImGui::Begin("gltf demo");
            ImGui::Text("Application average %.1f FPS", ImGui::GetIO().Framerate);
            ImGui::End();
        }

        scene_with_previous_scene::render();
    }

    void gltf_demo::reheat() {
        m_renderer.set_clear_color(vec4(0,0,0,1));
    }

} // scene_demos
