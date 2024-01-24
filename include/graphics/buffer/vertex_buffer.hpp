#ifndef GLSTUFF_VERTEX_BUFFER_HPP
#define GLSTUFF_VERTEX_BUFFER_HPP

#include "buffer.hpp"

#include <array>

namespace gl {
    class vertex_buffer {
        buffer m_buf;
        size_t m_stride;
    public:
        vertex_buffer(vertex_buffer&& o)  noexcept;
        vertex_buffer(buffer buf, size_t stride);

        vertex_buffer(const void* data, size_t size, size_t stride, buffer_creation_params params = {});
        vertex_buffer(size_t size, size_t stride, buffer_creation_params params = {});

        template<scluk::concepts::iterable arr_t>
        vertex_buffer(const arr_t& arr, buffer_creation_params params = {}) : vertex_buffer(buffer(arr, params), sizeof(typename arr_t::value_type)) {}

        template<scluk::concepts::indexable arr_t>
        void update(const arr_t &arr, size_t offset = 0) {
            assert(sizeof(typename arr_t::value_type) == m_stride);
            m_buf.update(offset, arr.data(), arr.size() * m_stride);
        }

        uint get_gl_id() const;
        size_t get_stride() const;
        void set_stride(size_t stride);
    };

} // gl

#endif //GLSTUFF_VERTEX_BUFFER_HPP
