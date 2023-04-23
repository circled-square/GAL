#ifndef GLSTUFF_VERTEX_ARRAY_HPP
#define GLSTUFF_VERTEX_ARRAY_HPP

#include <GL/glew.h>
#include "types.hpp"
#include "vertex_buffer.hpp"
#include "index_buffer.hpp"
#include <scluk/format.hpp>

namespace gl {

    class vertex_array {
        static constexpr uint vao_vbo_bind_index = 0; // arbitrary; simply 0 since we will only be using one vbo on this vao
        uint m_vao;
    public:
        vertex_buffer vbo;
        index_buffer ibo;
        vertex_array(vertex_buffer vbo, index_buffer ibo);
        ~vertex_array();

        void bind() const; // bind the vao
        void unbind() const; // unbind the vao

    private:
        void specify_single_attrib(uint attrib_index, uint offset, uint type, uint size);

        template<typename T>
        inline void specify_attribs_internal_helper(uint& attrib_index, uint& offset) {
            using vec = gl::scalar_to_vector<T>;
            uint type_id = gl::gl_type_id<typename vec::value_type>::v;
            uint size = vec::length();

            scluk::out("specify single_attrib(attr_index=%, offset=%, type_id=%, size=%)", attrib_index, offset, type_id, size);
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
