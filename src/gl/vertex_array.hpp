#ifndef GLSTUFF_VERTEX_ARRAY_HPP
#define GLSTUFF_VERTEX_ARRAY_HPP

#include <tuple>

#include <GL/glew.h>
#include "types.hpp"
#include "vertex_buffer.hpp"
#include "index_buffer.hpp"
#ifdef DEBUG_BUILD
#include <scluk/format.hpp>
#endif

namespace gl {

    class vertex_array {
        static constexpr uint vao_vbo_bind_index = 0; // arbitrary; simply 0 since we will only be using one vbo on this vao
        uint m_vao;
        vertex_array(vertex_buffer vbo, index_buffer ibo);
    public:
        vertex_buffer vbo;
        index_buffer ibo;

        vertex_array(vertex_array&& o) : m_vao(o.m_vao), vbo(std::move(o.vbo)), ibo(std::move(o.ibo)) {
            o.m_vao = 0;
        }

        template<typename vertex_t>
        static vertex_array make(vertex_buffer vbo, index_buffer ibo) {
            static_assert(sizeof(vertex_t) == vertex_t::layout_t::vertex_size(), "the vertex_t type passed to vertex_array::make has a size different than the one advertised by vertex_t::layout_t"); // validate the vertex layout
            vertex_array ret(std::move(vbo), std::move(ibo));
            vertex_t::layout_t::specify_attribs(ret);

            return ret;
        }

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

            //scluk::out("specify_single_attrib(attr_index=%, offset=%, type_id=%, size=%)", attrib_index, offset, type_id, size);

            specify_single_attrib(attrib_index, offset, type_id, size);

            attrib_index++;
            offset += sizeof(T);
        }
    public:
        template<typename... Ts>
        inline void specify_attribs(uint starting_index = 0, uint starting_offset = 0) {
            (specify_attribs_internal_helper<Ts>(starting_index, starting_offset), ...);
        }

    };

    template <typename... Ts>
    struct vertex_layout {
        using tuple_t = std::tuple<Ts...>;

        static void specify_attribs(gl::vertex_array& vao, uint starting_index = 0, uint starting_offset = 0) {
            vao.specify_attribs<Ts...>(starting_index, starting_offset);
        }
        constexpr static size_t vertex_size() { return sizeof(std::tuple<Ts...>); }

        constexpr vertex_layout(Ts...) {}
    };
}

#endif //GLSTUFF_VERTEX_ARRAY_HPP
