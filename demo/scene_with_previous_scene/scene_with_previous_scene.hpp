#ifndef GLSTUFF_SCENE_WITH_PREVIOUS_SCENE_HPP
#define GLSTUFF_SCENE_WITH_PREVIOUS_SCENE_HPP

#include "../../include/application/scene.hpp"


// scene_with_previous_scene is a thin wrapper over scene which shows an imgui window with a button to go back to the previous scene; useful especially to go back to the menu_demo after visualizing a demo


namespace scene_demos {

    class scene_with_previous_scene : public application::scene {
        application::scene* m_previous_scene;
    public:
        virtual void render();

        void set_prev_scene(application::scene* prev);
    };
} // scene_demos

#endif //GLSTUFF_SCENE_WITH_PREVIOUS_SCENE_HPP
