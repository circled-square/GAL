//
// Created by billy on 4/23/23.
//

#include "renderer.hpp"

namespace gl {
    void renderer::draw(const vertex_array &vao, const shader_program &shader, size_t ibo_start, size_t ibo_count) {
        shader.bind();
        vao.bind();

        glDrawElements(GL_TRIANGLES, ibo_count * 3, GL_UNSIGNED_INT, (const void*)(sizeof(uint) * 3 * ibo_start)); //draw triangles({1}), read {2} elements of type {3} starting from the {4}th byte from the bound GL_ELEMENT_ARRAY_BUFFER

        shader.unbind();
        vao.unbind();
    }

    void renderer::clear() {
        glClear(GL_COLOR_BUFFER_BIT);
    }
} // gl