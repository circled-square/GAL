#include "vertex_array.hpp"
#include <GL/glew.h>

namespace gl {

    vertex_array::vertex_array(vertex_buffer vbo, index_buffer ibo) : vbo(std::move(vbo)), ibo(std::move(ibo)) {
        glCreateVertexArrays(1, &m_vao);

        // bind the ibo {2} to the vao {1}
        glVertexArrayElementBuffer(m_vao, this->ibo.get_gl_id());
        // bind {3} as the {2}th vbo to the vao {1}, starting from {4} within the buffer. the stride between vertices is {5}
        glVertexArrayVertexBuffer(m_vao, vao_vbo_bind_index, this->vbo.get_gl_id(), 0, (int)this->vbo.get_stride());
    }

    vertex_array::~vertex_array() {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }

    void vertex_array::specify_single_attrib(uint attrib_index, uint offset, uint type, uint size) {
        //the vertex array contains vertices with the following attribs:
        glEnableVertexArrayAttrib(m_vao, attrib_index); //enable the attrib {2} for the vao {1} (can be done after call to glVertexAttribPointer I think)
        glVertexArrayAttribFormat(m_vao, attrib_index, size, type, GL_FALSE, offset); //the {1} vao's attrib 0({2}) is 2({3}) floats({4}); they do not({5}) need normalization; {6} is the offset of the attrib from the start of the vertex.
        glVertexArrayAttribBinding(m_vao, attrib_index, vao_vbo_bind_index); // bind the attrib {2} to the {3}th vbo of the vao {1}
    }

    void vertex_array::bind() {
        glBindVertexArray(m_vao);
    }

    void vertex_array::unbind() {
        glBindVertexArray(0);
    }
}