#include "image.hpp"
#include <stb/stb_image.h>

namespace stb {
    image::image(const char *filename) {
        stbi_set_flip_vertically_on_load(1);
        buffer = stbi_load(filename, &w, &h, &channels, 4);
    }

    image::~image() {
        stbi_image_free(buffer);
    }
}