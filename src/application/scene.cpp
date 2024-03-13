#include <GAL/application/scene.hpp>
#include <GAL/application/application.hpp>

namespace gal::application {
    scene::scene() : m_application(nullptr) {}

    void scene::raise_resize_event(glm::ivec2 resolution) {
        glViewport(0, 0, resolution.x, resolution.y);
        this->on_resize(resolution);
    }
    void scene::raise_key_press_event(int key, int scancode, int action, int mods) {
        this->on_key_press(key, scancode, action, mods);
    }

    void scene::change_scene(scene* s) {
        s->m_application = m_application;
        m_application->set_active_scene(s);
    }

}
