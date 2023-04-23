#ifndef GL_SHADER_HPP
#define GL_SHADER_HPP

#include "types.hpp"

#include <string>
#include <unordered_map>
#include <scluk/language_extension.hpp>
#include <scluk/functional.hpp>
#include <glm/glm.hpp>

#include <GL/glew.h>
namespace gl {
    using scluk::uint;

    //wrapper for a shader program. none of the functions need the shader to be bound
    class shader_program {
        uint m_program_id;
        std::unordered_map<std::string, int> m_uniform_location_cache;
        static uint compile_shader(uint type, const std::string& source);
    public:
        shader_program(const std::string& vert_shader, const std::string& frag_shader);
        ~shader_program();
        void bind() const; //sets the program as active
        void unbind() const;
        int get_uniform_location(const std::string& name);
        //overload this as needed
        void set_uniform(const char* name, auto v) {
            set_uniform(get_uniform_location(name), v);
        }

        int get_id() { return m_program_id; }

        template<typename T>
        void set_uniform(int uniform_location, T v) {
            //void glProgramUniform4fv(GLuint program, GLint location, GLsizei count, const GLfloat *value);
            (type_to_uniform_func<T>())(m_program_id, uniform_location, 1, &v);
        }

#ifdef DEBUG_BUILD
        uint get_gl_program_id() { return m_program_id; }
#endif
    private:
        template<typename T>
        static constexpr scluk::fnptr_t<void(uint, int, int, void*)> type_to_uniform_func() {
            using ret_t = scluk::fnptr_t<void(uint, int, int, void*)>;
            using arr_t = std::array<ret_t, 4>;
            using vec_t = scalar_to_vector<T>;
            using val_t = vec_t::value_type;
            static constexpr uint id = gl_type_id<val_t>::v;
            static constexpr uint vec_size = vec_t::length();

            const auto array = [](auto a, auto b, auto c, auto d) {
                return arr_t{ ret_t(a), ret_t(b), ret_t(c), ret_t(d) };
            };
            return
                (
                    id == GL_FLOAT ? array(glProgramUniform1fv, glProgramUniform2fv, glProgramUniform3fv, glProgramUniform4fv) :
                    id == GL_DOUBLE ? array(glProgramUniform1dv, glProgramUniform2dv, glProgramUniform3dv, glProgramUniform4dv) :
                    id == GL_INT || id == GL_BOOL ? array(glProgramUniform1iv, glProgramUniform2iv, glProgramUniform3iv, glProgramUniform4iv) :
                    id == GL_UNSIGNED_INT ? array(glProgramUniform1uiv, glProgramUniform2uiv, glProgramUniform3uiv, glProgramUniform4uiv) :
                    array(nullptr, nullptr, nullptr, nullptr)
                ) [vec_size - 1];
        }
    };
}

#endif //GL_SHADER_HPP
