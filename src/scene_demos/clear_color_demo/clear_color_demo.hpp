#ifndef GLSTUFF_CLEAR_COLOR_DEMO_HPP
#define GLSTUFF_CLEAR_COLOR_DEMO_HPP

#include "../../gl/renderer.hpp"
#include "../scene_with_previous_scene/scene_with_previous_scene.hpp"

#include <glm/glm.hpp>

namespace scene_demos {

    class clear_color_demo : public scene_with_previous_scene {
        gl::renderer m_renderer;
        glm::vec4 m_color;
    public:
        clear_color_demo();
        void render() final;

        void reheat() final;
    };
};

#endif //GLSTUFF_CLEAR_COLOR_DEMO_HPP
