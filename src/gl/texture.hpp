#ifndef GLSTUFF_TEXTURE_HPP
#define GLSTUFF_TEXTURE_HPP

#include <glad/glad.h>
#include "types.hpp"

namespace gl {
    class texture {
        uint m_texture_id;
        int m_width, m_height, m_components;
    public:
        texture(const void* buffer, int w, int h, int components, int alignment = 4);
        texture(texture&& o);
        ~texture();

        void set_texture_data(const void* buffer, int alignment = 4);

        void bind(uint slot = 0) const;
        void unbind(uint slot = 0) const;

        int get_width () const { return m_width; }
        int height() const { return m_height; }
    };
}


#endif //GLSTUFF_TEXTURE_HPP
