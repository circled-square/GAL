//
// Created by billy on 4/23/23.
//

#ifndef GLSTUFF_RENDERER_HPP
#define GLSTUFF_RENDERER_HPP

#include "vertex_array.hpp"
#include "shader.hpp"

namespace gl {

    class renderer {
    public:
        void draw(const vertex_array& vao, const shader_program& shader);
        void draw(const vertex_array& vao, const shader_program& shader, size_t ibo_start, size_t ibo_count);
        void clear();
        void set_clear_color(glm::vec4 c);

    };

} // gl

#endif //GLSTUFF_RENDERER_HPP
