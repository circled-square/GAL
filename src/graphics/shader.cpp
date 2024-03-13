#include <GAL/graphics/shader.hpp>
#include <internal/graphics/types.hpp>
#include <scluk/exception.hpp>
#include <scluk/log.hpp>
#include <vector>

namespace gal::graphics {
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
            throw scluk::runtime_error(
                "Failed to compile % shader\n\t%",
                type == GL_VERTEX_SHADER ? "vertex" : "fragment",
                gl_message.data()
            );

        }

        return id;
    }

    shader_program::shader_program(const std::string &vert_shader, const std::string &frag_shader) {
        this->m_program_id = glCreateProgram();
        uint vs = compile_shader(GL_VERTEX_SHADER, vert_shader);
        uint fs = compile_shader(GL_FRAGMENT_SHADER, frag_shader);

        glAttachShader(m_program_id, vs);
        glAttachShader(m_program_id, fs);

        glLinkProgram(m_program_id);
        glValidateProgram(m_program_id);

        glDeleteShader(vs);
        glDeleteShader(fs);
    }

    shader_program::shader_program(shader_program&& o) {
        m_program_id = o.m_program_id;
        o.m_program_id = 0;
        m_uniform_location_cache = std::move(o.m_uniform_location_cache);
    }

    shader_program::~shader_program() {
        glDeleteProgram(m_program_id);
    }

    void shader_program::bind() const {
        glUseProgram(m_program_id);
    }

    void shader_program::unbind() const {
#ifdef DEBUG_BUILD
        glUseProgram(0);
#endif
    }
    int shader_program::get_uniform_location(const std::string& name) {
        if(auto it = m_uniform_location_cache.find(name); it != m_uniform_location_cache.end()) {
            return it->second;
        }

        //cache miss
        int location = glGetUniformLocation(m_program_id, name.c_str());
        if(location == -1)
            scluk::stdout_log("WARNING: could not retrieve uniform with name %", name);

        m_uniform_location_cache.insert({ name, location });

        return location;
    }
}
