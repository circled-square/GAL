#ifndef GAL_APPLICATION_HPP
#define GAL_APPLICATION_HPP

#include "window/window.hpp"
#include <glm/glm.hpp>

#include "scene.hpp"

namespace gal::application {
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

#endif // GAL_APPLICATION_HPP
