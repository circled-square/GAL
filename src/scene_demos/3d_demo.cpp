#define THREE_D_DEMO

#include "3d_demo.hpp"
#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <scluk/read_file.hpp>
#include <scluk/format.hpp>

using namespace glm;
using scluk::read_file;

namespace scene_demos {

    constexpr vec3 x_axis = vec3(1,0,0), y_axis = vec3(0,1,0), z_axis = vec3(0,0,1);
    constexpr float pi = glm::pi<f32>();

    struct vertex_t {
        vec3 pos;
        vec2 tex_coord; // vec of floats because it will be used as a variant

        using layout_t = decltype(gl::static_vertex_layout(pos, tex_coord));
    };

    static constexpr std::array<vertex_t, 8> vertex_data {
        vertex_t
        { { .5,  .5,  .5}, {1., 1.} },
        { { .5, -.5,  .5}, {0., 1.} },
        { {-.5,  .5,  .5}, {0., 1.} },
        { {-.5, -.5,  .5}, {1., 1.} },

        { { .5,  .5, -.5}, {1., 0.} },
        { { .5, -.5, -.5}, {0., 0.} },
        { {-.5,  .5, -.5}, {0., 0.} },
        { {-.5, -.5, -.5}, {1., 0.} },
    };
    static constexpr std::array<uvec3, 12> indices {
        uvec3
        {0, 1, 2},
        {2, 3, 1},

        {4, 5, 6},
        {6, 7, 5},

        {0, 1, 4},
        {4, 5, 1},

        {1, 3, 5},
        {5, 7, 3},

        {2, 3, 6},
        {6, 7, 3},

        {0, 2, 4},
        {4, 6, 2},
    };
    three_dimensional_demo::three_dimensional_demo()
        : m_vao(gl::vertex_array::make<vertex_t>(gl::vertex_buffer(vertex_data), gl::index_buffer(indices))),
          m_shader(read_file("src/shader/3d_demo/vert.glsl"), read_file("src/shader/3d_demo/frag.glsl")),
          m_img("src/resources/example.png"),
          m_tex(m_img.buffer, m_img.w, m_img.h, 4),
          m_renderer(),
          //compute the mvp matrix
          m_model_mat(make_model_matrix(vec3(0,0 ,-1), pi/2, .25)),
          m_view_mat(make_view_matrix(vec3(0,0,0), -pi/2)),
          m_proj_mat(make_proj_matrix(vec2(4,3))),
          m_mvp_mat(m_proj_mat * m_view_mat * m_model_mat)
    {}

    mat4 three_dimensional_demo::make_model_matrix(vec3 pos, float rotation, float scale_factor) {
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

    mat4 three_dimensional_demo::make_view_matrix(vec3 pos, float rotation) {
        return rotate(
            translate(
                    mat4(1.f),
                    -pos
            ),
            -rotation,
            z_axis
        );
    }

    mat4 three_dimensional_demo::make_proj_matrix(vec2 aspect_ratio) {
        return perspective(pi / 4, aspect_ratio.x / aspect_ratio.y, .1f, 100.f);
        //return infinitePerspectiveLH(pi<f32>() / 2, aspect_ratio.x / aspect_ratio.y, -100000.f);
        //return ortho(-1, +1, -1, +1, -1, +1);
    }

    void three_dimensional_demo::update(float delta) {
        // edit the MVPs...
        m_model_mat = rotate(m_model_mat,  delta * pi / 16, y_axis);
        m_model_mat = rotate(m_model_mat,  delta * pi / 8, z_axis);
        /*static float time = 0.f;
        time += delta;
        m_model_mat = translate(m_model_mat, vec3(1,0,0) * delta * sin(time));*/
        //m_model_mat = translate(m_model_mat, vec3(0,0,1) * delta);

        m_mvp_mat = m_proj_mat * m_view_mat * m_model_mat;


        /*for(int i = 0; i < vertex_data.size(); i++) {
            vec4 v = m_mvp_mat * vec4(vertex_data[i].pos, 1);
            scluk::out("vert %, (%, %, %, %)", i, v.x, v.y, v.z, v.w);
        }*/
    }

    void three_dimensional_demo::render() {
        m_renderer.clear();

        m_shader.set_uniform("u_mvp", m_mvp_mat);
        m_renderer.draw(m_vao, m_shader);

        {
            ImGui::Begin("Multiple object draw demo");
            ImGui::Text("Application average %.1f FPS", ImGui::GetIO().Framerate);
            ImGui::End();
        }

        //render the "take me back" menu window
        scene_with_previous_scene::render();
    }

    void three_dimensional_demo::reheat() {
        m_renderer.set_clear_color(vec4(0,0,0,1));
        int texture_slot = 0;
        m_tex.bind(texture_slot);
        m_shader.set_uniform<int>("u_texture_slot", texture_slot);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendEquation(GL_FUNC_ADD);
        glEnable(GL_BLEND);

        // Enable depth test
        glEnable(GL_DEPTH_TEST);
        // Accept fragment if it closer to the camera than the former one
        glDepthFunc(GL_LESS);
    }
}
