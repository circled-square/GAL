#ifndef GLSTUFF_BUFFER_HPP
#define GLSTUFF_BUFFER_HPP

#include <glad/glad.h>
#include "../../internal/graphics/types.hpp"
#include <scluk/metaprogramming.hpp>

namespace gl {
    struct buffer_creation_params {
        bool is_static = true;
        // apply when is_static = true
        uint flags = GL_DYNAMIC_STORAGE_BIT;
        // apply when is_static = false
        uint hints = GL_DYNAMIC_DRAW;
    };

    class buffer {
        uint m_buf_id;
    public:
        buffer(const void* data, size_t size, buffer_creation_params params = { });

        template<scluk::concepts::iterable arr_t>
        buffer(const arr_t& arr, buffer_creation_params params = {})
            : buffer(arr.data(), arr.size() * sizeof(typename arr_t::value_type), params) {}

        buffer(buffer&& o)  noexcept;

        ~buffer();

        void update(size_t offset, const void *data, size_t size);


        bool is_null() const { return m_buf_id == 0; }
        bool operator!() const { return is_null(); }
        operator bool() const { return !is_null(); }

        uint get_gl_id() const { return m_buf_id; }
    };
} // gl

#endif //GLSTUFF_BUFFER_HPP
