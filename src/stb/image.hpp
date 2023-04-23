//
// Created by billy on 4/23/23.
//

#ifndef GLSTUFF_IMAGE_HPP
#define GLSTUFF_IMAGE_HPP

#include <stb/stb_image.h>

namespace stb {
    struct image {
        int w, h, channels;
        void* buffer;

        image(const char* filename);

        ~image();
    };

} // stb

#endif //GLSTUFF_IMAGE_HPP
