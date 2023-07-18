//
// Created by billy on 4/23/23.
//

#include "renderer.hpp"

namespace gl {
    void renderer::draw(const vertex_array& vao, const shader_program& shader, uint ibo_index) {
        draw(vao, shader, 0, vao.get_triangle_count(ibo_index), ibo_index);
    }
    void renderer::draw(const vertex_array &vao, const shader_program &shader, size_t ibo_start, size_t ibo_count, uint ibo_index) {
        shader.bind();
        vao.bind(ibo_index);

        glDrawElements(GL_TRIANGLES, ibo_count * 3, GL_UNSIGNED_INT, (const void*)(sizeof(uint) * 3 * ibo_start)); //draw triangles({1}), read {2} elements of type {3} starting from the {4}th byte from the bound GL_ELEMENT_ARRAY_BUFFER

        shader.unbind();
        vao.unbind();
    }

    void renderer::clear() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void renderer::set_clear_color(glm::vec4 c) {
        glClearColor(c.x, c.y, c.z, c.w);
    }
} // gl
