#ifndef GLSTUFF_SCENE_LOADER_HPP
#define GLSTUFF_SCENE_LOADER_HPP

#include "scene.hpp"
#include "../glfw/window.hpp"

namespace scene_loader {

    //this class does not own pointers to any scene and won't call delete on them
    class scene_loader {
        glfw::window& m_window;
        scene* m_active_scene;
    public:
        scene_loader(glfw::window& window, scene* active_scene);

        //returns previously active scene
        scene* set_active_scene(scene* new_scene);

        void run();
    };

} // scene_demos

#endif //GLSTUFF_SCENE_LOADER_HPP
