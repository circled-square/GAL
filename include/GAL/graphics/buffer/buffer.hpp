#ifndef GAL_GRAPHICS_BUFFER_HPP
#define GAL_GRAPHICS_BUFFER_HPP

#include <glad/glad.h>
#include <internal/graphics/types.hpp>
#include <scluk/metaprogramming.hpp>

namespace gal::graphics {
    struct buffer_creation_params {
        bool is_static = true;
        // apply when is_static = true => glNamedBufferStorage
        uint flags = GL_DYNAMIC_STORAGE_BIT;
        // apply when is_static = false => glNamedBufferData
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
        uint get_gl_id() const { return m_buf_id; }
    };
}

#endif //GAL_GRAPHICS_BUFFER_HPP
