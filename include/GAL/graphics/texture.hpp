#ifndef GAL_GRAPHICS_TEXTURE_HPP
#define GAL_GRAPHICS_TEXTURE_HPP

#include "image.hpp"
#include <internal/graphics/types.hpp>
#include <random>

namespace gal::graphics {
    class framebuffer; //only to declare as friend of texture
    class texture {
        friend class framebuffer;
        uint m_texture_id;
        int m_width, m_height, m_components;
    public:
        struct specification {
            int w, h;
            int components;

            const void* data = nullptr;
            int alignment = 4;

            bool repeat_wrap = false;
            GLenum filter_method = GL_NEAREST;
            //TODO: specification is missing a parameter to allow the user to have more than 8 bit depth for each component
        };
        texture(const specification& spec);
        texture(const image& image);
        texture(texture&& o);
        ~texture();

        void set_texture_data(const void* buffer, int alignment = 4);

        void bind(uint slot) const;
        void unbind(uint slot) const;

        int width () const;
        int height() const;
        glm::ivec2 resolution() const;

        //returns a square size x size texture containing grayscale noise
        template<size_t bit_depth>
        static texture noise(glm::ivec2 res, char components) {
            std::random_device rand_dev;
            std::subtract_with_carry_engine<unsigned short, bit_depth, 20, 48> rng(rand_dev());

            unsigned char* data = new unsigned char[res.x*res.y*components];

            for (int y = 0; y < res.y; y++) {
                for (int x = 0; x < res.x; x++) {
                    for (char c = 0; c < components; c++) {
                        data[c + x*components + y*res.x*components] = rng() << (8 - bit_depth);
                    }
                }
            }
            specification spec = {
                .w = res.x, .h = res.y, .components=components, .data=data, .alignment=1, .repeat_wrap=true,
            };

            return texture(spec);
        }
    };
}


#endif //GAL_GRAPHICS_TEXTURE_HPP
