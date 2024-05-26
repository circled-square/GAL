#ifndef GAL_GRAPHICS_FRAMEBUFFER_HPP
#define GAL_GRAPHICS_FRAMEBUFFER_HPP

#include "texture.hpp"
#include <stdexcept>

namespace gal::graphics {

    // this class does not use openGL Direct State Access simply because it doesn't seem to work the way you'd expect with FBOs and renderbuffers
    class framebuffer {
        unsigned int m_fbo;
        texture m_tex;
        unsigned int m_depth_renderbuf_id;
    public:

        class construction_exception : std::exception {
            unsigned int m_code;
            std::string m_what;
        public:
            construction_exception(unsigned int error_code);
            virtual const char* what() const noexcept;
            unsigned int get_code() const noexcept;
        };

        framebuffer(const framebuffer&) = delete;
        framebuffer() = delete;
        framebuffer(framebuffer&& o);

        //may throw construction_exception
        framebuffer(const texture::specification& spec);
        ~framebuffer();

        void bind_draw();
        void bind_read();
        void bind();
        static void unbind();

        texture& get_texture();
        const texture& get_texture() const;
    };
}

#endif // GAL_GRAPHICS_FRAMEBUFFER_HPP
