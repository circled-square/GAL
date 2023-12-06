#include "menu_demo.hpp"
#include "scene_with_previous_scene.hpp"
#include <glad/glad.h>
#include <imgui.h>

namespace scene_demos {
    menu_demo::menu_demo(std::vector<named_scene> scene_vector)
            : m_scene_vector(std::move(scene_vector)) {}

    void menu_demo::update(float delta) {}

    void menu_demo::render() {
        m_renderer.clear();

        {
            ImGui::Begin("Scene Menu");

            for(named_scene& s : m_scene_vector) {
                if(ImGui::Button(s.name.c_str())) {
                    m_current_scene.reset(s.scene_constructor());
                    this->change_scene(m_current_scene.get());
                    m_current_scene->set_prev_scene(this);
                }
            }

            ImGui::Text("Application average %.1f FPS", ImGui::GetIO().Framerate);
            ImGui::End();
        }
    }

    void menu_demo::reheat() {
        m_current_scene.reset(); // destroy the current scene as soon as this one is reheated

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendEquation(GL_FUNC_ADD);
        glEnable(GL_BLEND);
        m_renderer.set_clear_color(glm::vec4(0,0.1,0,1));
    }

    menu_demo::menu_demo() : menu_demo(std::vector<named_scene>()) {}

    void menu_demo::add_scene(named_scene s) {
        m_scene_vector.push_back(std::move(s));
    }

    void menu_demo::add_scene(named_scene::scene_ctor_t scene_ctor, std::string name) {
        add_scene({scene_ctor, std::move(name)});
    }

    void menu_demo::activate(const char *scene_name) {
        for(named_scene& s : m_scene_vector) {
            if(s.name == scene_name) {
                m_current_scene.reset(s.scene_constructor());
                this->change_scene(m_current_scene.get());
                m_current_scene->set_prev_scene(this);
                return;
            }
        }
        throw scluk::runtime_error("menu_demo::activate called with string '%'; no such named scene", scene_name);
    }
}
