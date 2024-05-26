#ifndef GAL_GRAPHICS_TEXTURE_HPP
#define GAL_GRAPHICS_TEXTURE_HPP

#include "image.hpp"
#include <internal/graphics/types.hpp>

namespace gal::graphics {
    class framebuffer; //only to declare as friend of texture
    class texture {
        friend class framebuffer;
        uint m_texture_id;
        int m_width, m_height, m_components;
    public:
        struct specification {
            int w, h;
            int components;

            const void* data = nullptr;
            int alignment = 4;

            bool repeat_wrap = false;
            GLenum filter_method = GL_NEAREST;
            //TODO: specification is missing a parameter to allow the user to have more than 8 bit depth for each component
        };
        texture(const specification& spec);
        texture(const image& image);
        texture(texture&& o);
        ~texture();

        void set_texture_data(const void* buffer, int alignment = 4);

        void bind(uint slot) const;
        void unbind(uint slot) const;

        int get_width () const { return m_width; }
        int height() const { return m_height; }
    };
}


#endif //GAL_GRAPHICS_TEXTURE_HPP
