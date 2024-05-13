#ifndef GAL_GRAPHICS_RETRO_RENDERER_HPP
#define GAL_GRAPHICS_RETRO_RENDERER_HPP

#include "renderer.hpp"
#include "../vertex_array.hpp"
#include "../shader.hpp"
#include "../texture.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <optional>


// forward declaration to declare friend of retro_renderable without including
namespace gal::application::retro {
    class scene;
}

namespace gal::graphics::retro {
    class renderable;
    class camera;

    class renderer {
        gal::graphics::renderer m_renderer;
        shader_program m_shader;

    public:
        renderer();
        void draw(const renderable &renderable, const camera &camera);
        void clear();
        void set_clear_color(glm::vec4 c);
    };

    //vertex type expected by the shader used by retro_renderer
    struct vertex_t {
        glm::vec3 pos;
        glm::vec2 tex_coord;

        using layout_t = decltype(gal::graphics::static_vertex_layout(pos, tex_coord));
    };

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


    //simple implementation of renderable interface, usually sufficient
    class retro_renderable : public renderable {
        vertex_array m_vao;
        texture m_tex;
        glm::mat4 m_model;

        //mutable access to model matrix only permitted to friend class gal::application::retro::scene
        void set_model_mat(glm::mat4& m);
        friend class gal::application::retro::scene;
    public:
        retro_renderable(retro_renderable &&o);
        retro_renderable(vertex_array vao, texture tex, glm::mat4 model_mat = glm::mat4(1));
        virtual const vertex_array& get_vao() const final;
        virtual const texture& get_texture() const final;
        virtual void set_uniforms(gal::graphics::shader_program &shader, const camera &camera) const final;

        const glm::mat4& get_model_mat();
    };
}

#endif //GAL_GRAPHICS_RETRO_RENDERER_HPP
