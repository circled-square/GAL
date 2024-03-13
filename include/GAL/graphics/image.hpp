#ifndef GAL_GRAPHICS_IMAGE_HPP
#define GAL_GRAPHICS_IMAGE_HPP


namespace gal::graphics {
    struct image {
        int w, h, channels;
        void* buffer;

        image(const char* filename);
        image(image&& o);

        ~image();
    };

}

#endif //GAL_GRAPHICS_IMAGE_HPP
