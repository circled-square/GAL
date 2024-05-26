#include <GAL/graphics/framebuffer.hpp>
#include <glad/glad.h>
#include <unordered_map>
#include <format>

namespace gal::graphics {
    framebuffer::framebuffer(const texture::specification &spec) : m_tex(spec) {
        glGenFramebuffers(1, &m_fbo);
        bind();

        // create depth buffer
        glGenRenderbuffers(1, &m_depth_renderbuf_id);
        glBindRenderbuffer(GL_RENDERBUFFER, m_depth_renderbuf_id);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, spec.w, spec.h);

        // attach depth buffer
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth_renderbuf_id);

        // attach the texture to which we will be rendering
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_tex.m_texture_id, 0);// attach the texture {3}'s mipmap {4} to framebuffer {0} at attach index {1}

        // Set the list of draw buffers.
        unsigned int draw_buffers[1] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, draw_buffers);

        // Check for creation errors
        unsigned int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            framebuffer::unbind(); // limit side effects to a minimum
            throw construction_exception(status);
        }

        framebuffer::unbind(); // limit side effects to a minimum
    }

    framebuffer::framebuffer(framebuffer &&o): m_fbo(o.m_fbo), m_tex(std::move(o.m_tex)), m_depth_renderbuf_id(o.m_depth_renderbuf_id) {
        o.m_fbo = o.m_depth_renderbuf_id = -1;
    }

    framebuffer::~framebuffer() {
        glDeleteFramebuffers(1, &m_fbo);
        glDeleteRenderbuffers(1, &m_depth_renderbuf_id);
    }

    void framebuffer::bind_draw() {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    }

    void framebuffer::bind_read() {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
    }

    void framebuffer::bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    }

    void framebuffer::unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    texture &framebuffer::get_texture() { return m_tex; }

    const texture &framebuffer::get_texture() const { return m_tex; }




    // construction_exception

    framebuffer::construction_exception::construction_exception(unsigned int error_code) : m_code(error_code) {
        std::unordered_map<unsigned int, std::string> glenum_to_string = {
            { GL_FRAMEBUFFER_UNDEFINED, 						"GL_FRAMEBUFFER_UNDEFINED" },
            { GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT, 			"GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" },
            { GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT,  	"GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" },
            { GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER, 			"GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER" },
            { GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER,			"GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER" },
            { GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE, 			"GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE" },
            { GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS,			"GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS" },
            { GL_FRAMEBUFFER_UNSUPPORTED,						"GL_FRAMEBUFFER_UNSUPPORTED" }
        };

        m_what = std::format("error during framebuffer object construction: {} ({})", glenum_to_string[m_code], m_code);
    }

    unsigned int framebuffer::construction_exception::get_code() const noexcept { return m_code; }

    const char* framebuffer::construction_exception::what() const noexcept { return m_what.c_str(); }
}
