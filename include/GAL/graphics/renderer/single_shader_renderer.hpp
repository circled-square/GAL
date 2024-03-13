#ifndef GAL_GRAPHICS_SINGLE_SHADER_RENDERER_HPP
#define GAL_GRAPHICS_SINGLE_SHADER_RENDERER_HPP

#include "renderer.hpp"
#include "../vertex_array.hpp"
#include "../shader.hpp"

namespace gal::graphics {
    class single_shader_renderer {
        shader_program m_shader;
        renderer m_renderer;
    public:
        single_shader_renderer(shader_program shader);
        void draw(const vertex_array& vao, uint ibo_index = 0);
        void draw(const vertex_array& vao, size_t ibo_start, size_t ibo_count, uint ibo_index = 0);
        void clear();
        void set_clear_color(glm::vec4 c);
    };
}

#endif //GAL_GRAPHICS_SINGLE_SHADER_RENDERER_HPP
