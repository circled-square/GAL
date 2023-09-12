#ifndef APPLICATION_APPLICATION_HPP
#define APPLICATION_APPLICATION_HPP

#include "glfw/window.hpp"

#include "scene.hpp"

namespace application {
    class application {
        glfw::window m_window;
        scene* m_active_scene;
    public:
        application(glm::ivec2 res, const std::string& title, scene* active_scene);

        ~application();

        glfw::window& get_window() { return m_window; }
        const glfw::window& get_window() const { return m_window; }


        scene* set_active_scene(scene* new_scene);

        void run();
    };
}

#endif
