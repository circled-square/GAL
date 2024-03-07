#include "scene.hpp"
#include "application.hpp"

namespace gal::application {
    scene::scene() : m_application(nullptr) {}

    scene* scene::change_scene(scene* s) {
        s->m_application = m_application;
        return m_application->set_active_scene(s);
    }

}
