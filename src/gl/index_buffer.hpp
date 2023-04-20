#ifndef GLSTUFF_INDEX_BUFFER_HPP
#define GLSTUFF_INDEX_BUFFER_HPP

#include "buffer.hpp"

namespace gl {

    template<typename T>
    concept array_of_uvec3 = (scluk::concepts::indexable<T> && sizeof(typename T::value_type) == 3 * sizeof(uint));

    class index_buffer {
        buffer m_buf;
        static constexpr size_t stride = 3 * sizeof(uint);
        index_buffer(buffer buf) : m_buf(std::move(buf)) {}
    public:
        index_buffer(index_buffer&& o)  noexcept : index_buffer(std::move(o.m_buf)) {}

        template<array_of_uvec3 arr_t>
        index_buffer(const arr_t& arr, buffer_creation_params params = {}) : index_buffer(buffer(arr, params)) { }

        index_buffer(const void* data, size_t size, buffer_creation_params params = {}) : index_buffer(buffer(data, size, params)) { }
        index_buffer(size_t size, buffer_creation_params params = {}) : index_buffer(nullptr, size, params) { }

        template<array_of_uvec3 arr_t>
        void update(const arr_t& arr, size_t offset = 0) {
            m_buf.update(offset, arr.data(), arr.size() * stride);
        }

        uint get_gl_id() { return m_buf.get_gl_id(); }
    };
} // gl

#endif //GLSTUFF_INDEX_BUFFER_HPP
