#include "index_buffer.hpp"

using namespace glm;

namespace gl {
    index_buffer::index_buffer(buffer buf, size_t size) : m_buf(std::move(buf)), m_triangle_count(size) {}

    index_buffer::index_buffer(index_buffer &&o) noexcept: index_buffer(std::move(o.m_buf), o.m_triangle_count) {}

    index_buffer::index_buffer(const uvec3* data, size_t size, buffer_creation_params params) : index_buffer(buffer(data, size*sizeof(uvec3), params), size) { }

    index_buffer::index_buffer(size_t size, buffer_creation_params params) : index_buffer(nullptr, size, params) { }

    uint index_buffer::get_gl_id() const { return m_buf.get_gl_id(); }

    uint index_buffer::get_triangle_count() const { return m_triangle_count; }
}
