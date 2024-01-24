#include "image.hpp"
#include <stb/stb_image.h>
#include <cassert>

namespace gal::graphics {
    image::image(const char *filename) {
        stbi_set_flip_vertically_on_load(1);
        buffer = stbi_load(filename, &w, &h, &channels, 4);
    }

    image::image(image&& o) {
        w = o.w;
        h = o.h;
        channels = o.channels;
        buffer = o.buffer;

        o.buffer = nullptr;
    }

    image::~image() {
        assert(buffer);
        stbi_image_free(buffer);
        buffer = nullptr;
    }
}
