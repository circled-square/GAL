//
// Created by billy on 4/23/23.
//

#ifndef GLSTUFF_TEXTURE_HPP
#define GLSTUFF_TEXTURE_HPP

#include <GL/glew.h>
#include "types.hpp"

namespace gl {
    class texture {
        uint m_texture_id;
        int m_width, m_height, m_bytes_per_pixel;
    public:
        texture(const void* buffer, int w, int h, int bytes_per_pixel);
        ~texture();

        void set_texture_data(const void* buffer);

        void bind(uint slot = 0) const;
        void unbind(uint slot = 0) const;

        int get_width () const { return m_width; }
        int height() const { return m_height; }
    };
}


#endif //GLSTUFF_TEXTURE_HPP
