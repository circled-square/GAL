#ifndef GL_SHADER_HPP
#define GL_SHADER_HPP

#include <string>
#include <scluk/aliases.hpp>

namespace gl {
    using scluk::uint;
    uint create_shader(const std::string& vertexSh, const std::string& fragmentSh);
    uint compile_shader(uint type, const std::string& source);

    class shader_program {
        uint m_gl_program;
        static uint compile_shader(uint type, const std::string& source);
    public:
        shader_program(const std::string& vert_shader, const std::string& frag_shader);
        ~shader_program();
        void use(); //sets the program as active
    };
}

#endif //GL_SHADER_HPP
