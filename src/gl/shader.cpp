#include "shader.hpp"
#include <scluk/language_extension.hpp>
#include <GL/glew.h>
#include <stdexcept>
#include <vector>

using namespace scluk;

namespace gl {
    uint shader_program::compile_shader(uint type, const std::string &source) {
        uint id = glCreateShader(type);
        const char *src = source.c_str();
        glShaderSource(id, 1, &src, nullptr);
        glCompileShader(id);

        int compilation_result;
        glGetShaderiv(id, GL_COMPILE_STATUS, &compilation_result);
        if (!compilation_result) {
            int length;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
            std::vector<char> gl_message(length);
            glGetShaderInfoLog(id, length, &length, gl_message.data());

            glDeleteShader(id);
            std::string error_message = sout(
                    "Failed to compile % shader\n\t%",
                    type == GL_VERTEX_SHADER ? "vertex" : "fragment",
                    gl_message.data()
            );
            throw std::runtime_error(error_message);
        }

        return id;
    }

    shader_program::shader_program(const std::string &vert_shader, const std::string &frag_shader) {
        this->m_gl_program = glCreateProgram();
        uint vs = compile_shader(GL_VERTEX_SHADER, vert_shader);
        uint fs = compile_shader(GL_FRAGMENT_SHADER, frag_shader);

        glAttachShader(m_gl_program, vs);
        glAttachShader(m_gl_program, fs);

        glLinkProgram(m_gl_program);
        glValidateProgram(m_gl_program);

        glDeleteShader(vs);
        glDeleteShader(fs);
    }

    void shader_program::use() {
        glUseProgram(m_gl_program);
    }
    shader_program::~shader_program() {
        glDeleteProgram(m_gl_program);
    }
}