//
// Created by billy on 4/23/23.
//

#ifndef GLSTUFF_IMAGE_HPP
#define GLSTUFF_IMAGE_HPP


namespace stb {
    struct image {
        int w, h, channels;
        void* buffer;

        image(const char* filename);

        ~image();
    };

} // image

#endif //GLSTUFF_IMAGE_HPP
