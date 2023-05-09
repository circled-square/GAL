#include "scene.hpp"
#include "scene_loader.hpp"

namespace scene_loader {
    scene::scene() : m_scene_loader(nullptr) {}

    scene::~scene() {}

    scene* scene::change_scene(scene* s) {
        s->m_scene_loader = m_scene_loader;
        return m_scene_loader->set_active_scene(s);
    }

}
