#include "texture_demo.hpp"
#include <glm/glm.hpp>
#include <scluk/read_file.hpp>
#include <array>
#include <imgui.h>

using namespace glm;
using scluk::read_file;


namespace scene_demos {
    struct vertex_t {
        vec2 pos;
        vec2 tex_coord;

        using layout_t = decltype(gl::vertex_layout(pos, tex_coord));
    };
    static constexpr std::array<vertex_t, 4> vertex_data {
            vertex_t{ { .5,  .5}, {1., 1.} },
            { { .5, -.5}, {1., 0.} },
            { {-.5, -.5}, {0., 0.} },
            { {-.5,  .5}, {0., 1.} }
    };
    static constexpr std::array<uvec3, 2> indices {
            uvec3{0, 1, 2},
            {2, 3, 0}
    };

    texture_demo::texture_demo()
            : m_vao(gl::vertex_array::make<vertex_t>(gl::vertex_buffer(vertex_data), gl::index_buffer(indices))),
              m_shader(read_file("src/shader/texture_demo/vert.glsl"), read_file("src/shader/texture_demo/frag.glsl")),
              m_renderer(),
              m_img("src/resources/example.png"),
              m_tex(m_img.buffer, m_img.w, m_img.h, 4)
    {}
    void texture_demo::update(float delta) {}

    void texture_demo::render() {
        m_renderer.clear();

        m_shader.set_uniform("u_color", vec4(.8, .4, .4, 1.0));
        m_shader.set_uniform("u_mvp", glm::mat4(1));
        m_renderer.draw(m_vao, m_shader);

        {
            ImGui::Begin("texture demo");
            ImGui::Text("Application average %.1f FPS", ImGui::GetIO().Framerate);
            ImGui::End();
        }

        scene_with_previous_scene::render();
    }

    void texture_demo::reheat() {
        m_renderer.set_clear_color(vec4(0,0,0,1));
        int texture_slot = 0;
        m_tex.bind(texture_slot);
        m_shader.set_uniform<int>("u_texture_slot", texture_slot);
    }

} // scene_demos