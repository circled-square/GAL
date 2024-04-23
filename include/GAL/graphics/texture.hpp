#ifndef GAL_GRAPHICS_TEXTURE_HPP
#define GAL_GRAPHICS_TEXTURE_HPP

#include <glad/glad.h>
#include "image.hpp"
#include <internal/graphics/types.hpp>

namespace gal::graphics {
    class texture {
        uint m_texture_id;
        int m_width, m_height, m_components;
    public:
        struct specification {
            int w, h;
            int components;

            const void* data = nullptr;
            int alignment = 4;

            bool repeat_wrap = false;
        };
        texture(const specification& spec);
        texture(const image& image);
        texture(texture&& o);
        ~texture();

        void set_texture_data(const void* buffer, int alignment = 4);

        void bind(uint slot = 0) const;
        void unbind(uint slot = 0) const;

        int get_width () const { return m_width; }
        int height() const { return m_height; }
    };
}


#endif //GAL_GRAPHICS_TEXTURE_HPP
