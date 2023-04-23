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
        int m_width, m_height, m_bits_per_pixel;
    public:
        texture(const void* buffer, int w, int h, int bits_per_pixel)
                : m_texture_id(0), m_width(w), m_height(h), m_bits_per_pixel(bits_per_pixel) {
            glCreateTextures(GL_TEXTURE_2D, 1, &m_texture_id);

            //obligatory parameters
            glTextureParameteri(m_texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(m_texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(m_texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(m_texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            //init the immutable storage
            glTextureStorage2D(m_texture_id, 1, GL_RGBA8, m_width, m_height);

            //write on the immutable storage
            if(buffer)
                set_texture_data(buffer);
        }
        ~texture() {
            glDeleteTextures(1, &m_texture_id);
        }

        void set_texture_data(const void* buffer) {
            glTextureSubImage2D(m_texture_id, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
        }

        void bind(uint slot = 0) const {
            glBindTextureUnit(slot, m_texture_id);
        }
        void unbind(uint slot = 0) const {
#ifdef DEBUG_BUILD
            glBindTextureUnit(slot, 0);
#endif
        }

        int get_width () const { return m_width; }
        int height() const { return m_height; }
    };
}


#endif //GLSTUFF_TEXTURE_HPP
