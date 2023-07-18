#include "mul_obj_draw_demo.hpp"
#include <glad/glad.h>
#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <scluk/read_file.hpp>

using namespace glm;
using scluk::read_file;

namespace scene_demos {
    inline namespace mul_obj_draw_demo_namespace {
        struct vertex_t {
            vec2 pos;
            vec2 tex_coord;

            using layout_t = decltype(gl::static_vertex_layout(pos, tex_coord));
        };
    }

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
    mul_obj_draw_demo::mul_obj_draw_demo()
        : m_vao(gl::vertex_array::make<vertex_t>(gl::vertex_buffer(vertex_data), gl::index_buffer(indices))),
          m_shader(read_file("src/shader/mul_obj_draw_demo/vert.glsl"), read_file("src/shader/mul_obj_draw_demo/frag.glsl")),
          m_renderer(),
          m_img("src/resources/example.png"),
          m_tex(m_img.buffer, m_img.w, m_img.h, 4),
          //compute the mvp matrices
          m_model_mat_1(make_model_matrix(vec3(0.,.4,0), 1, .5)),
          m_model_mat_2(make_model_matrix(vec3(.3,0,0), 3, .5)),
          m_view_mat(make_view_matrix(vec3(0,0,0), 0)),
          m_proj_mat(make_proj_matrix(vec2(4,3))),
          m_mvp_mat_1(m_proj_mat * m_view_mat * m_model_mat_1),
          m_mvp_mat_2(m_proj_mat * m_view_mat * m_model_mat_2)
    {}

    mat4 mul_obj_draw_demo::make_model_matrix(vec3 pos, float rotation, float scale_factor) {
        vec3 z_axis = vec3(0, 0, 1);

        return scale(
            rotate(
                translate(
                    mat4(1),
                    pos
                ),
                rotation,
                z_axis
            ),
            vec3(scale_factor)
        );
    }

    mat4 mul_obj_draw_demo::make_view_matrix(vec3 pos, float rotation) {
        vec3 z_axis = vec3(0, 0, 1);

        return rotate(
            translate(
                    mat4(1.f),
                    -pos
            ),
            -rotation,
            z_axis
        );
    }

    mat4 mul_obj_draw_demo::make_proj_matrix(vec2 aspect_ratio) {
        //make aspect ratio have 1 as the smaller one
        aspect_ratio /= std::min(aspect_ratio.x, aspect_ratio.y);

        return ortho(-aspect_ratio.x, aspect_ratio.x, -aspect_ratio.y, aspect_ratio.y, -1.f, 1.f);
    }

    void mul_obj_draw_demo::update(float delta) {
        // edit the MVPs...
        vec3 z_axis = vec3(0,0,1);
        float pi = glm::pi<f32>();
        m_model_mat_1 = rotate(m_model_mat_1,  delta * pi / 2, z_axis);
        m_model_mat_2 = rotate(m_model_mat_2, -delta * pi / 2, z_axis);
        m_view_mat = rotate(m_view_mat, delta * pi / 6, z_axis);
        m_mvp_mat_1 = m_proj_mat * m_view_mat * m_model_mat_1;
        m_mvp_mat_2 = m_proj_mat * m_view_mat * m_model_mat_2;
    }

    void mul_obj_draw_demo::render() {
        m_renderer.clear();

        m_shader.set_uniform("u_color", vec4(.8, .4, .4, 1.0));
        m_shader.set_uniform("u_mvp", m_mvp_mat_1);
        m_renderer.draw(m_vao, m_shader);

        m_shader.set_uniform("u_color", vec4(.4, .4, .8, .5));
        m_shader.set_uniform("u_mvp", m_mvp_mat_2);
        m_renderer.draw(m_vao, m_shader);


        {
            ImGui::Begin("Multiple object draw demo");
            ImGui::Text("Application average %.1f FPS", ImGui::GetIO().Framerate);
            ImGui::End();
        }

        //render the "take me back" menu window
        scene_with_previous_scene::render();
    }

    void mul_obj_draw_demo::reheat() {

        m_renderer.set_clear_color(vec4(0,0,0,1));
        int texture_slot = 0;
        m_tex.bind(texture_slot);
        m_shader.set_uniform<int>("u_texture_slot", texture_slot);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendEquation(GL_FUNC_ADD);
        glEnable(GL_BLEND);

        glDisable(GL_DEPTH_TEST);
    }

}
