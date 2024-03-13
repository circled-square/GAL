#include <GAL/graphics/renderer/single_shader_renderer.hpp>
#include <internal/graphics/types.hpp>

#include <scluk/log.hpp>

namespace gal::graphics {
    single_shader_renderer::single_shader_renderer(shader_program shader) 
        : m_shader(std::move(shader)) {}

    void single_shader_renderer::draw(const vertex_array& vao, uint ibo_index) {
        m_renderer.draw(vao, m_shader, ibo_index);
    }
    void single_shader_renderer::draw(const vertex_array& vao, size_t ibo_start, size_t ibo_count, uint ibo_index) {
        m_renderer.draw(vao, m_shader, ibo_start, ibo_count, ibo_index);
    }

    void single_shader_renderer::clear() {
        m_renderer.clear();
    }

    void single_shader_renderer::set_clear_color(glm::vec4 c) {
        m_renderer.set_clear_color(c);
    }
}
