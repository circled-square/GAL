//
// Created by billy on 4/23/23.
//

#define STB_IMAGE_IMPLEMENTATION
#include "image.hpp"

namespace stb {
    image::image(const char *filename) {
        stbi_set_flip_vertically_on_load(1);
        buffer = stbi_load(filename, &w, &h, &channels, 4);
    }

    image::~image() {
        stbi_image_free(buffer);
    }
} // stb