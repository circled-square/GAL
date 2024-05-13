#include <GAL/graphics/renderer/retro_renderer.hpp>
#include <internal/graphics/types.hpp>

#include <scluk/log.hpp>

namespace gal::graphics::retro {
    static shader_program make_retro_shader() {
        const char *frag = "#version 330 core \n\
            in vec2 v_tex_coord; \
            uniform sampler2D u_texture_slot; \
            out vec4 color; \
            \
            void main() { \
                color = texture(u_texture_slot, v_tex_coord); \
        }";
        const char *vert = "#version 440 core \n\
            layout(location = 0) in vec3 pos; \
            layout(location = 1) in vec2 tex_coord; \
            out vec2 v_tex_coord; \
            uniform mat4 u_mvp; \
            \
            void main() { \
                gl_Position = u_mvp * vec4(pos, 1); \
                v_tex_coord = tex_coord; \
        }";

        return shader_program(vert, frag);
    }

    renderer::renderer()
        : m_shader(make_retro_shader()) {}

    void renderer::clear() {
        m_renderer.clear();
    }

    void renderer::set_clear_color(glm::vec4 c) {
        m_renderer.set_clear_color(c);
    }

    void renderer::draw(const renderable &renderable,
                                      const camera &camera) {
        renderable.set_uniforms(m_shader, camera);
        m_renderer.draw(renderable.get_vao(), m_shader,
                        renderable.get_ibo_start(), renderable.get_ibo_count(),
                        renderable.get_ibo_index());
    }





    static glm::mat4 make_proj_matrix(glm::ivec2 aspect_ratio) {
        return glm::perspective(glm::pi<float>() / 4, (float)aspect_ratio.x / aspect_ratio.y, .1f, 100.f);
    }
    camera::camera(glm::vec3 position, glm::vec3 direction, glm::ivec2 aspect_ratio)
        : m_pos(position), m_direction(direction), m_proj(make_proj_matrix(aspect_ratio)) {}

    glm::mat4 camera::get_view_mat() const {
        if (!m_view)
            m_view = glm::lookAt(m_pos, m_pos + m_direction, glm::vec3(0, 1, 0));
        return *m_view;
    }

    glm::mat4 camera::get_proj_mat() const { return m_proj; }

    glm::mat4 camera::get_viewproj_mat() const { return get_proj_mat() * get_view_mat(); }

    void camera::set_aspect_ratio(glm::ivec2 aspect_ratio) { m_proj = make_proj_matrix(aspect_ratio); }

    glm::vec3 camera::get_position() const { return m_pos; }

    glm::vec3 camera::get_direction() const { return m_direction; }

    void camera::set_position(glm::vec3 pos) {
        m_pos = pos;
        m_view.reset();
    }

    void camera::set_direction(glm::vec3 dir) {
        m_direction = dir;
        m_view.reset();
    }
    simple_renderable::simple_renderable(vertex_array vao, texture tex,
                                         glm::mat4 model_mat)
        : m_vao(std::move(vao)), m_tex(std::move(tex)), m_model(model_mat) {}
    simple_renderable::simple_renderable(simple_renderable &&o)
        : simple_renderable(std::move(o.m_vao), std::move(o.m_tex), o.m_model) {
    }
    const vertex_array &simple_renderable::get_vao() const { return m_vao; }
    const texture &simple_renderable::get_texture() const { return m_tex; }
    glm::mat4 &simple_renderable::get_model_mat() { return m_model; }
    void simple_renderable::set_uniforms(gal::graphics::shader_program &shader,
                                         const camera &camera) const {
        const int texture_slot = 0;
        m_tex.bind(texture_slot);
        shader.set_uniform("u_texture_slot", texture_slot);
        shader.set_uniform("u_mvp", camera.get_viewproj_mat() * m_model);
    }
    } // namespace gal::graphics
