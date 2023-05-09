#include "scene_loader.hpp"

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>
#include <chrono>

namespace scene_loader {
    scene_loader::scene_loader(glfw::window &window, scene* active_scene)
            : m_window(window),
              m_active_scene(active_scene) {
        m_active_scene->m_scene_loader = this;
    }

    scene* scene_loader::set_active_scene(scene* new_scene) {
        scene* old_active_scene = m_active_scene;
        m_active_scene = new_scene;

        m_active_scene->m_scene_loader = this;
        old_active_scene->m_scene_loader = nullptr;

        return old_active_scene;
    }

    void scene_loader::run() {
        auto last_frame_time = std::chrono::high_resolution_clock::now();  //used to calculate the delta
        while (!m_window.should_close() && !ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape))) {
            auto curr_frame_time = std::chrono::high_resolution_clock::now();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            float delta = std::chrono::duration<float, std::ratio<1>>(curr_frame_time - last_frame_time).count();

            m_active_scene->update(delta);
            m_active_scene->render();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            m_window.swap_buffers();
            m_window.poll_events();

            last_frame_time = curr_frame_time;
        }
    }

} // scene_demos