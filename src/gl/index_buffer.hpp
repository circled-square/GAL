#ifndef GLSTUFF_INDEX_BUFFER_HPP
#define GLSTUFF_INDEX_BUFFER_HPP

#include "buffer.hpp"
#include <glm/glm.hpp>

namespace gl {
    template<typename T>
    concept array_of_uvec3 = scluk::concepts::indexable<T> && std::same_as<typename T::value_type, glm::uvec3>;

    class index_buffer {
        buffer m_buf;
        size_t m_triangle_count; // store the size of the buffer to be passed to opengl for draw calls  (unless only part of the ibo is read)
        static constexpr size_t stride = 3 * sizeof(uint);
        index_buffer(buffer buf, size_t size);
    public:
        index_buffer(index_buffer&& o)  noexcept;

        template<array_of_uvec3 arr_t>
        index_buffer(const arr_t& arr, buffer_creation_params params = {}) : index_buffer(buffer(arr, params), arr.size()) { }

        //pass in the number of uvec3, NOT the bytes
        index_buffer(const glm::uvec3* data, size_t size, buffer_creation_params params = {});
        index_buffer(size_t size, buffer_creation_params params = {});

        template<array_of_uvec3 arr_t>
        void update(const arr_t& arr, size_t offset = 0) {
            m_buf.update(offset, arr.data(), arr.size() * stride);
        }

        uint get_gl_id();
        uint get_triangle_count() const;
    };
} // gl

#endif //GLSTUFF_INDEX_BUFFER_HPP
