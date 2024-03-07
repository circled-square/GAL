#ifndef GAL_APPLICATION_HPP
#define GAL_APPLICATION_HPP

#include "window/window.hpp"
#include <glm/glm.hpp>

#include "scene.hpp"

namespace gal::application {

    //use glfw backend (the only one)
    namespace window = window_glfw;

    class application {
        window::window m_window;
        scene* m_active_scene;

    public:
        application(application&&) = delete; // application cannot be moved: m_window's window handle points to it in its user pointer and scene::m_application points to it

        application(glm::ivec2 res, const std::string& title, scene* active_scene);

        ~application();

        window::window& get_window() { return m_window; }
        const window::window& get_window() const { return m_window; }

        scene* set_active_scene(scene* new_scene);

        void run();
    };
}

#endif // GAL_APPLICATION_HPP
