#ifndef GAL_GRAPHICS_SINGLE_SHADER_RENDERER_HPP
#define GAL_GRAPHICS_SINGLE_SHADER_RENDERER_HPP

#include "renderer.hpp"
#include "../vertex_array.hpp"
#include "../shader.hpp"
#include "../texture.hpp"
#include <unordered_map>

namespace gal::graphics {
    class renderable {
    public:
        virtual ~renderable() {}

        virtual void set_uniforms(gal::graphics::shader_program& shader) const {}
        virtual const vertex_array& get_vao() const = 0;
        virtual const texture& get_texture() const = 0;
        virtual uint get_ibo_index() const { return 0; }
        virtual size_t get_ibo_start() const { return 0; }
        virtual size_t get_ibo_count() const { return get_vao().get_triangle_count(get_ibo_index()); }
    };


    // possible usage
    /// TODO: delete this
    /*
    struct resource {
        int ref_count;
        vertex_array vao;
        texture texture;
    };

    class entity : renderable {
        glm::mat4 m_mvp;
        resource& m_resource;
    public:
        entity(resource& resource, glm::mat4 mvp) : m_mvp(mvp), m_resource(resource) {
            m_resource.ref_count++;
        }
        ~entity() {}
        virtual const vertex_array& get_vao() const {
            return m_resource.vao;
        }
        virtual const texture& get_texture() const {
            return m_resource.texture;
        }
        virtual void set_uniforms(gal::graphics::shader_program& shader) const {
            shader.set_uniform("u_mvp", m_mvp);
            const int texture_slot = 0;
            m_resource.texture.bind(texture_slot);
            shader.set_uniform("u_texture_slot", texture_slot);
        }
    };



    class resource_sharing_entity_system {
        std::unordered_map<std::string, resource> resources;
    public:
        void instantiate(const char* name) {
            if (resources.contains(name)) {
                auto& resource = resources[name];
                resource.ref_count++;
                resource.vao;
                resource.texture;
            }
        }
    };*/

    class single_shader_renderer {
        renderer m_renderer;
    public:
        shader_program m_shader; // temporarily public until i figure out the interface

        single_shader_renderer(shader_program shader);
        void draw(const renderable& renderable) {
            renderable.set_uniforms(m_shader);
            m_renderer.draw(renderable.get_vao(), m_shader, renderable.get_ibo_start(), renderable.get_ibo_count(), renderable.get_ibo_index());
        }
        void draw(const vertex_array& vao, uint ibo_index = 0);
        void draw(const vertex_array& vao, size_t ibo_start, size_t ibo_count, uint ibo_index = 0);
        void clear();
        void set_clear_color(glm::vec4 c);
    };
}

#endif //GAL_GRAPHICS_SINGLE_SHADER_RENDERER_HPP
