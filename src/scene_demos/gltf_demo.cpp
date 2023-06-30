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
        
        if (!success)
          out("Fatal error: Failed to parse glTF");
    }
    void gltf_demo::update(float delta) {}

    void gltf_demo::render() {
        m_renderer.clear();

        //m_shader.set_uniform("u_color", vec4(.8, .4, .4, 1.0));
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
        //m_shader.set_uniform<int>("u_texture_slot", texture_slot);
    }

} // scene_demos
