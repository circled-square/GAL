#ifndef GL4RENDERINGENGINE_PRINT_VEC_HPP
#define GL4RENDERINGENGINE_PRINT_VEC_HPP

#include <ostream>
#include <glm/glm.hpp>

template<int size, typename T, glm::qualifier q>
inline std::ostream& fmt_vec_to_stream(std::ostream& os, glm::vec<size, T, q> v) {
    os << "(";
    for(int i = 0; i < size-1; i++)
        os << v[i] << ", ";
    os << v[size-1] << ")";

    return os;
}

inline std::ostream& operator<<(std::ostream& os, glm::uvec3 v) { return fmt_vec_to_stream(os, v); }
inline std::ostream& operator<<(std::ostream& os, glm::u16vec3 v) { return fmt_vec_to_stream(os, v); }

#endif //GL4RENDERINGENGINE_PRINT_VEC_HPP
