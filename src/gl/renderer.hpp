#ifndef GLSTUFF_RENDERER_HPP
#define GLSTUFF_RENDERER_HPP

#include "vertex_array.hpp"
#include "shader.hpp"

namespace gl {

    class renderer {
    public:
        void draw(const vertex_array& vao, const shader_program& shader, uint ibo_index = 0);
        void draw(const vertex_array& vao, const shader_program& shader, size_t ibo_start, size_t ibo_count, uint ibo_index = 0);
        void clear();
        void set_clear_color(glm::vec4 c);

    };

} // gl

#endif //GLSTUFF_RENDERER_HPP
