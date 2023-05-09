#include "menu_demo.hpp"
#include "scene_with_previous_scene.hpp"
#include <GL/glew.h>
#include <imgui.h>

namespace scene_demos {
    menu_demo::menu_demo(std::vector<named_scene> scene_vector)
            : m_scene_vector(std::move(scene_vector)) {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendEquation(GL_FUNC_ADD);
        glEnable(GL_BLEND);
    }

    void menu_demo::update(float delta) {}

    void menu_demo::render() {
        m_renderer.clear();

        {
            ImGui::Begin("Scene Menu");                          // Create a window called "Hello, world!" and append into it.

            for(named_scene& s : m_scene_vector) {
                if(ImGui::Button(s.name.c_str())) {
                    scene_with_previous_scene& new_scene = dynamic_cast<scene_with_previous_scene&>(*s.scene);

                    new_scene.set_prev_scene(this->change_scene(s.scene.get()));
                }
            }

            ImGui::Text("Application average %.1f FPS", ImGui::GetIO().Framerate);
            ImGui::End();
        }
    }

}