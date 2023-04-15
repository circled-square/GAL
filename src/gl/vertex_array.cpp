#include "vertex_array.hpp"
#include <GL/glew.h>

namespace gl {

    vertex_array::vertex_array() {
        glCreateVertexArrays(1, &m_vao);
        uint buf_ids[2];
        glCreateBuffers(2, buf_ids);
        m_vbo = buf_ids[0];
        m_ibo = buf_ids[1];
    }

    vertex_array::~vertex_array() {
        glDeleteVertexArrays(1, &m_vao);
        uint buffers[2] = {m_vbo, m_ibo };
        glDeleteBuffers(2, buffers);
        m_vao = m_vbo = m_ibo = 0;
    }
    void vertex_array::init_ibo(const void* data, size_t size, uint flags) {
        // bind a {2} bytes memory area to the currently bound array({1}) buffer; write {2} bytes reading from {3}; usage hints are passed in {4}
        // "Storage" makes this storage immutable in size

        glNamedBufferStorage(m_ibo, size, data, flags);

        // bind the ibo {2} to the vao {1}
        glVertexArrayElementBuffer(m_vao, m_ibo);
    }

    void vertex_array::init_vbo(const void* data, size_t stride, size_t size, uint flags) {
        // bind a {2} bytes memory area to the currently bound array({1}) buffer; write {2} bytes reading from {3}; usage hints are passed in {4}
        // storage makes this storage immutable in size
        glNamedBufferStorage(m_vbo, size, data, flags);
        //glNamedBufferData(m_vbo, size, data, GL_DYNAMIC_DRAW);

        //bind {3} as the {2}th vbo to the vao {1}, starting from {4} within the buffer. the stride between vertices is {5}
        glVertexArrayVertexBuffer(m_vao, vao_vbo_bind_index, m_vbo, 0, stride);
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

    void vertex_array::update_vbo(size_t offset, const void *data, size_t size) {
        glNamedBufferSubData(m_vbo, offset, size, data);
    }
}