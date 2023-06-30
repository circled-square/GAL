#ifndef GLSTUFF_TYPES_HPP
#define GLSTUFF_TYPES_HPP

#include <glad/glad.h>
#include <scluk/aliases.hpp>
#include <type_traits>
#include <cassert>
#include <glm/glm.hpp>

namespace gl {
    using scluk::uint;
    static_assert(sizeof(uint) == sizeof(GLuint), "unsigned int and GLuint do not have the same size");

    //make a struct for each of the types that opengl defines as aliases of other types,
    //so they can be treated as distinct types and passed to templates
    namespace types {
        struct fixed_point { GLfixed v; };
        struct f16 { GLhalf v;};
    }

    template<typename T> struct gl_type_id {};
    template<> struct gl_type_id<bool>{ static constexpr uint v = GL_BOOL; };
    template<> struct gl_type_id<GLbyte>{ static constexpr uint v = GL_BYTE; };
    template<> struct gl_type_id<GLubyte> { static constexpr uint v = GL_UNSIGNED_BYTE; };
    template<> struct gl_type_id<GLshort> { static constexpr uint v = GL_SHORT; };
    template<> struct gl_type_id<GLushort> { static constexpr uint v = GL_UNSIGNED_SHORT; };
    template<> struct gl_type_id<GLint> { static constexpr uint v = GL_INT; };
    template<> struct gl_type_id<GLuint> { static constexpr uint v = GL_UNSIGNED_INT; };
    template<> struct gl_type_id<float> { static constexpr uint v = GL_FLOAT; };
    template<> struct gl_type_id<double> { static constexpr uint v = GL_DOUBLE; };
    template<> struct gl_type_id<types::f16> { static constexpr uint v = GL_HALF_FLOAT; };
    template<> struct gl_type_id<types::fixed_point> { static constexpr uint v = GL_FIXED; };


    template<typename T> concept glm_vector = requires {
        { T::length() } -> std::convertible_to<size_t>;
        { std::declval<T>().x } -> std::convertible_to<typename T::value_type>;
    };
    template<typename T> concept glm_matrix = requires {
        { std::declval<typename T::transpose_type>() };
    };
    static_assert(!glm_vector<float> &&  glm_vector<glm::vec2> && !glm_vector<glm::mat3>);
    static_assert(!glm_matrix<float> && !glm_matrix<glm::vec2> &&  glm_matrix<glm::mat3>);

    //scalar_to_vector<T> wraps any scalar type into its vec<1, T> form, but doesn't wrap T if it is a glm vector
    namespace internal {
        template<typename T>
        struct scalar_to_vector_helper {
            using type = glm::vec<1, T, glm::defaultp>;
        };
        template<glm_vector vec_t>
        struct scalar_to_vector_helper<vec_t>{
            using type = vec_t;
        };
    }
    
    template<typename T>
    using scalar_to_vector = internal::scalar_to_vector_helper<T>::type;

    //vector_to_scalar<T> transforms any vec<n, T> into T, and returns T if it is a scalar
    template<typename T>
    using vector_to_scalar = scalar_to_vector<T>::value_type;
}

#endif //GLSTUFF_TYPES_HPP
