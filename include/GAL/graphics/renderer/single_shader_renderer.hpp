#ifndef GAL_GRAPHICS_SINGLE_SHADER_RENDERER_HPP
#define GAL_GRAPHICS_SINGLE_SHADER_RENDERER_HPP

#include "renderer.hpp"
#include "../vertex_array.hpp"
#include "../shader.hpp"
#include "../texture.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <optional>

namespace gal::graphics {
    class renderable;
    class camera;

    class single_shader_renderer {
        renderer m_renderer;
        shader_program m_shader;

    public:
        single_shader_renderer(shader_program shader);
        void draw(const renderable &renderable, const camera &camera);
        void clear();
        void set_clear_color(glm::vec4 c);
    };

    single_shader_renderer make_retro_renderer();

    class camera {
        glm::vec3 m_pos, m_direction;
        glm::mat4 m_proj;

        //view matrix cache to avoid recomputing it unless pos/direction changed
        mutable std::optional<glm::mat4> m_view;
    public:
        camera(glm::vec3 position, glm::vec3 direction, glm::ivec2 aspect_ratio = {1, 1});

        glm::mat4 get_view_mat() const;
        glm::mat4 get_proj_mat() const;
        glm::mat4 get_viewproj_mat() const;

        glm::vec3 get_position() const;
        glm::vec3 get_direction() const;

        void set_position(glm::vec3 pos);
        void set_direction(glm::vec3 dir);
        void set_aspect_ratio(glm::ivec2 aspect_ratio);
    };

    class renderable {
    public:
        virtual ~renderable() {}

        virtual void set_uniforms(gal::graphics::shader_program& shader, const camera& camera) const {}
        virtual const vertex_array& get_vao() const = 0;
        virtual const texture& get_texture() const = 0;

        virtual uint get_ibo_index() const { return 0; }
        virtual size_t get_ibo_start() const { return 0; }
        virtual size_t get_ibo_count() const { return get_vao().get_triangle_count(get_ibo_index()); }
    };

    //example implementation of renderable interface, usually sufficient
    class simple_renderable : public renderable {
        vertex_array m_vao;
        texture m_tex;
        glm::mat4 m_model;
    public:
        simple_renderable(simple_renderable &&o);
        simple_renderable(vertex_array vao, texture tex, glm::mat4 model_mat = glm::mat4(1));
        virtual const vertex_array& get_vao() const final;
        virtual const texture& get_texture() const final;
        virtual void set_uniforms(gal::graphics::shader_program &shader, const camera &camera) const final;

        glm::mat4 &get_model_mat();
    };


}

#endif //GAL_GRAPHICS_SINGLE_SHADER_RENDERER_HPP
