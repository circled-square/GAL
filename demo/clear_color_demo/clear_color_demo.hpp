#ifndef GAL_CLEAR_COLOR_DEMO_HPP
#define GAL_CLEAR_COLOR_DEMO_HPP

#include "../../include/graphics/renderer/renderer.hpp"
#include "../scene_with_previous_scene/scene_with_previous_scene.hpp"

#include <glm/glm.hpp>

namespace scene_demos {

    class clear_color_demo : public scene_with_previous_scene {
        gal::graphics::renderer m_renderer;
        glm::vec4 m_color;
    public:
        clear_color_demo();
        void render() final;

        void reheat() final;
    };
};

#endif //GAL_CLEAR_COLOR_DEMO_HPP
