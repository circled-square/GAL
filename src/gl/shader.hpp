#ifndef GL_SHADER_HPP
#define GL_SHADER_HPP

#include <string>
#include <scluk/language_extension.hpp>
#include <glm/glm.hpp>

#include <GL/glew.h>
namespace gl {
    using scluk::uint;

    //wrapper for a shader program. none of the functions need the shader to be bound
    class shader_program {
        uint m_gl_program;
        static uint compile_shader(uint type, const std::string& source);
    public:
        shader_program(const std::string& vert_shader, const std::string& frag_shader);
        ~shader_program();
        void use(); //sets the program as active
        int get_uniform_location(const char* name);
        //overload this as needed
        void set_uniform(int uniform_location, glm::vec4 v);
        void set_uniform(const char* name, glm::vec4 v);

        int get_id() { return m_gl_program; }
    };
}

#endif //GL_SHADER_HPP
