#ifndef GAL_GRAPHICS_RENDERER_HPP
#define GAL_GRAPHICS_RENDERER_HPP

#include "../vertex_array.hpp"
#include "../shader.hpp"

namespace gal::graphics {
    class renderer {
    public:
        void draw(const vertex_array& vao, const shader_program& shader, uint ibo_index = 0);
        void draw(const vertex_array& vao, const shader_program& shader, size_t ibo_start, size_t ibo_count, uint ibo_index = 0);
        void clear();
        void set_clear_color(glm::vec4 c);
    };
}

#endif //GAL_GRAPHICS_RENDERER_HPP
