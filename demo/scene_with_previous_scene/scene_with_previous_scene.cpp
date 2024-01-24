#include "scene_with_previous_scene.hpp"
#include <imgui.h>

namespace scene_demos {
    void scene_with_previous_scene::render() {
        if(m_previous_scene) {
            ImGui::Begin("Go back to previous scene?");

            if(ImGui::Button("Yes, take me back")) {
                change_scene(m_previous_scene);
            }

            ImGui::End();
        }
    }

    void scene_with_previous_scene::set_prev_scene(gal::application::scene *prev) {
        m_previous_scene = prev;
    }
} // scene_demos
