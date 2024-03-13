#ifndef GAL_APPLICATION_SCENE_HPP
#define GAL_APPLICATION_SCENE_HPP

#include <glm/glm.hpp>

namespace gal::application {
    class application; // the file this is defined in is not included to avoid circular inclusion

    class scene {
        friend class application;
        application* m_application; // this is private: the only reason it is ever used is to change the active scene, and subclasses should do that through change_scene()
    public:
        scene();
        scene(scene&&) = delete;

        void raise_resize_event(glm::ivec2 resolution);
        void raise_key_press_event(int key, int scancode, int action, int mods);

        virtual ~scene() = default;
        virtual void update(float delta) {}
        virtual void render(glm::ivec2 resolution) {}

        virtual void freeze() {}
        virtual void reheat() {}

        //this possibly leaks this object
        void change_scene(scene* s);

    protected:
        virtual void on_key_press(int key, int scancode, int action, int mods) {}
        virtual void on_resize(glm::ivec2 resolution) {}
    };
};

#endif //GAL_APPLICATION_SCENE_HPP
