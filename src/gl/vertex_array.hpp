#ifndef GLSTUFF_VERTEX_ARRAY_HPP
#define GLSTUFF_VERTEX_ARRAY_HPP

#include <scluk/aliases.hpp>
#include <GL/glew.h>
#include "types.hpp"

namespace gl {

    class vertex_array {
        static constexpr uint vao_vbo_bind_index = 0; // arbitrary; simply 0 since we will only be using one vbo on this vao
        uint m_vao, m_vbo, m_ibo;
    public:
        vertex_array();
        ~vertex_array();

        void bind(); // bind the vao
        void unbind(); // unbind the vao

        //uses data.size() to determine how many bytes to copy
        void init_vbo(const void* data, size_t stride, size_t size, uint flags = GL_DYNAMIC_STORAGE_BIT);
        //uses data.size() to determine how many bytes to copy
        inline void init_vbo(const auto& data, uint flags = GL_DYNAMIC_STORAGE_BIT) {
            init_vbo(data.data(), sizeof(data[0]), data.size() * sizeof(data[0]), flags);
        }

        void update_vbo(size_t offset, const void* data, size_t size);
        void update_vbo(const auto& data) {
            update_vbo(0, data.data(), data.size() * sizeof(data[0]));
        }


        void init_ibo(const void* data, size_t size, uint flags = GL_DYNAMIC_STORAGE_BIT);
        //uses data.size() to determine how many bytes to copy
        inline void init_ibo(const auto& data, uint flags = GL_DYNAMIC_STORAGE_BIT) {
            init_ibo(data.data(), data.size() * sizeof(data[0]), flags);
        }

    private:
        void specify_single_attrib(uint attrib_index, uint offset, uint type, uint size);

        template<typename T>
        inline void specify_attribs_internal_helper(uint& attrib_index, uint& offset) {
            using vec = gl::scalar_to_vector<T>;
            uint type_id = gl::gl_type_id<decltype(vec::x)>::v;
            uint size = vec::length();

            specify_single_attrib(attrib_index, offset, type_id, size);

            attrib_index++;
            offset += sizeof(T);
        }
    public:
        template<typename...Ts>
        inline void specify_attribs(uint starting_index = 0, uint starting_offset = 0) {
            (specify_attribs_internal_helper<Ts>(starting_index, starting_offset), ...);
        }
    };

}

#endif //GLSTUFF_VERTEX_ARRAY_HPP
