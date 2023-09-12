#ifndef APPLICATION_APPLICATION_HPP
#define APPLICATION_APPLICATION_HPP

#include "../glfw/window.hpp"
#include "../gl/initialize_opengl.hpp"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "scene.hpp"

namespace application {
    class application {
        glfw::window m_window;
        scene* m_active_scene;
    public:
        application(glm::ivec2 res, const std::string& title, scene* active_scene) : m_window(res, title), m_active_scene(active_scene) {
            gl::initialize_opengl();

            ImGui::CreateContext();
            ImGui_ImplGlfw_InitForOpenGL(m_window, true);
            ImGui_ImplOpenGL3_Init();

            ImGui::StyleColorsDark();
            ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;


            m_active_scene->m_application = this;
            m_active_scene->reheat();
        }

        ~application() {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }

        glfw::window& get_window() { return m_window; }


        scene* set_active_scene(scene* new_scene) {
            scene* old_active_scene = m_active_scene;
            m_active_scene = new_scene;

            old_active_scene->freeze();
            old_active_scene->m_application = nullptr;

            m_active_scene->m_application = this;
            m_active_scene->reheat();

            return old_active_scene;
        }

        void run() {
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
    };
}

#endif
