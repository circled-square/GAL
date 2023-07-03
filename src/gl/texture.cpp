#include "texture.hpp"

namespace gl {

    texture::texture(const void *buffer, int w, int h, int components, int alignment)
            : m_texture_id(0), m_width(w), m_height(h), m_components(components) {
        assert(components >= 1 && components <= 4);

        glCreateTextures(GL_TEXTURE_2D, 1, &m_texture_id);

        //obligatory parameters
        glTextureParameteri(m_texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);



        //init the immutable storage
        glTextureStorage2D(m_texture_id, 1, GL_RGBA8, m_width, m_height);

        //write on the immutable storage
        if(buffer) {
            set_texture_data(buffer, alignment);
        }
    }

    texture::texture(texture&& o) {
        m_texture_id = o.m_texture_id;
        o.m_texture_id = 0;
        m_width = o.m_width;
        m_height = o.m_height;
        m_components = o.m_components;
    }

    texture::~texture() {
        glDeleteTextures(1, &m_texture_id);
    }

    void texture::set_texture_data(const void *buffer, int alignment) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);

        uint format = 
            m_components == 1 ? GL_RED :
            m_components == 2 ? GL_RG :
            m_components == 3 ? GL_RGB : GL_RGBA;

        glTextureSubImage2D(m_texture_id, 0, 0, 0, m_width, m_height, format, GL_UNSIGNED_BYTE, buffer);
    }

    void texture::bind(uint slot) const {
        glBindTextureUnit(slot, m_texture_id);
    }

    void texture::unbind(uint slot) const {
#ifdef DEBUG_BUILD
        glBindTextureUnit(slot, 0);
#endif
    }
}
