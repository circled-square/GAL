#ifndef GLSTUFF_VERTEX_BUFFER_HPP
#define GLSTUFF_VERTEX_BUFFER_HPP

#include "buffer.hpp"

namespace gl {
    class vertex_buffer {
        buffer m_buf;
        const size_t m_stride;
        vertex_buffer(buffer buf, size_t stride) : m_buf(std::move(buf)), m_stride(stride) {}
    public:
        vertex_buffer(vertex_buffer&& o)  noexcept : vertex_buffer(std::move(o.m_buf), o.m_stride) {}

        vertex_buffer(const void* data, size_t size, size_t stride, buffer_creation_params params = {}) : vertex_buffer(buffer(data, size, params), stride) { }
        vertex_buffer(size_t size, size_t stride, buffer_creation_params params = {}) : vertex_buffer(nullptr, size, stride, params) { }

        template<scluk::concepts::iterable arr_t>
        vertex_buffer(const arr_t& arr, buffer_creation_params params = {}) : vertex_buffer(buffer(arr, params), sizeof(typename arr_t::value_type)) {}

        template<scluk::concepts::indexable arr_t>
        void update(const arr_t &arr, size_t offset = 0) {
            assert(sizeof(typename arr_t::value_type) == m_stride);
            m_buf.update(offset, arr.data(), arr.size() * m_stride);
        }
        uint get_gl_id() { return m_buf.get_gl_id(); }
        size_t get_stride() { return m_stride; }
    };

} // gl

#endif //GLSTUFF_VERTEX_BUFFER_HPP
