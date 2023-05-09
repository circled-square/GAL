#include "clear_color_demo.hpp"

#include <imgui.h>

namespace scene_demos {
    void clear_color_demo::render() {
        m_renderer.clear();
        ImGui::Begin("Pick a color");
        glm::vec4 last_color = m_color;
        ImGui::ColorEdit4("", &m_color[0]);
        ImGui::End();

        if(last_color != m_color)
            m_renderer.set_clear_color(m_color);
        //render the "take me back" menu window
        scene_with_previous_scene::render();
    }

    void clear_color_demo::reheat() {
        m_renderer.set_clear_color(m_color);
    }

    clear_color_demo::clear_color_demo() : m_color(0,0,.1, 0) {}
}
