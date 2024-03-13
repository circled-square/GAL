#ifndef GAL_GRAPHICS_SHADER_HPP
#define GAL_GRAPHICS_SHADER_HPP

#include <internal/graphics/types.hpp>

#include <string>
#include <array>
#include <unordered_map>
#include <scluk/functional.hpp>
#include <glm/glm.hpp>

#include <glad/glad.h>

namespace gal::graphics {
    namespace internal {
        using uniform_func_t = scluk::fnptr_t<void(uint, int, int, void*)>;
        using uniform_mat_func_t = scluk::fnptr_t<void(uint, int, int, GLboolean, void*)>;

        //a sort of make_array, only working with pointers and returning an array of uniform_func_t
        template<typename... Ts>
        static constexpr std::array<void*, sizeof...(Ts)> make_array(Ts... args){
            return { (void*)args... };
        };
        //specialization for scalars and vectors
        template<typename T>
        struct type_to_uniform_func__struct {
            using vec_t = scalar_to_vector<T>;
            using val_t = vec_t::value_type;
            static constexpr uint id = gl_type_id<val_t>::v;
            static constexpr uint vec_size = vec_t::length();
            static uniform_func_t get() {
                return (uniform_func_t)(
                        id == GL_FLOAT ? make_array(glProgramUniform1fv, glProgramUniform2fv, glProgramUniform3fv, glProgramUniform4fv) :
                        id == GL_DOUBLE ? make_array(glProgramUniform1dv, glProgramUniform2dv, glProgramUniform3dv, glProgramUniform4dv) :
                        id == GL_INT || id == GL_BOOL ? make_array(glProgramUniform1iv, glProgramUniform2iv, glProgramUniform3iv, glProgramUniform4iv) :
                        id == GL_UNSIGNED_INT ? make_array(glProgramUniform1uiv, glProgramUniform2uiv, glProgramUniform3uiv, glProgramUniform4uiv) :
                        make_array(nullptr, nullptr, nullptr, nullptr)
                )[vec_size - 1];
            }
        };
        template<typename T>
        uniform_func_t type_to_uniform_func() { return type_to_uniform_func__struct<T>::get(); }

        //specialization for matrices
        template<typename T>
        struct type_to_uniform_mat_func__struct {  };
        template<typename T, int x, int y>
        struct type_to_uniform_mat_func__struct<glm::mat<x,y,T,glm::defaultp>> {
            static uniform_mat_func_t get() {
                return (uniform_mat_func_t)
                make_array(
                        make_array(glProgramUniformMatrix2fv, glProgramUniformMatrix2x3fv, glProgramUniformMatrix2x4fv)[y - 2],
                        make_array(glProgramUniformMatrix3x2fv, glProgramUniformMatrix3fv, glProgramUniformMatrix3x4fv)[y - 2],
                        make_array(glProgramUniformMatrix4x2fv, glProgramUniformMatrix4x3fv, glProgramUniformMatrix4fv)[y - 2]
                )[x - 2];
            }
        };
        template<typename T>
        uniform_mat_func_t type_to_uniform_mat_func() { return type_to_uniform_mat_func__struct<T>::get(); }


    }

    using scluk::uint;

    //wrapper for a shader program. none of the functions need the shader to be bound
    class shader_program {
        uint m_program_id;
        std::unordered_map<std::string, int> m_uniform_location_cache;
        static uint compile_shader(uint type, const std::string& source);
    public:
        shader_program(const std::string& vert_shader, const std::string& frag_shader);
        shader_program(shader_program&& o);
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
            using namespace internal;
            uniform_func_t uniform_func = type_to_uniform_func<T>();
            //void glProgramUniform4fv(GLuint program, GLint location, GLsizei count, const GLfloat *value);
            uniform_func(m_program_id, uniform_location, 1, &v);
        }
        template<glm_matrix T>
        void set_uniform(int uniform_location, T v) {
            using namespace internal;
            uniform_mat_func_t uniform_func = type_to_uniform_mat_func<T>();
            uniform_func(m_program_id, uniform_location, 1, GL_FALSE, &v);
        }

#ifdef DEBUG_BUILD
        uint get_gl_program_id() { return m_program_id; }
#endif
    };
}

#endif //GAL_GRAPHICS_SHADER_HPP