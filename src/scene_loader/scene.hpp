#ifndef GLSTUFF_SCENE_HPP
#define GLSTUFF_SCENE_HPP

#include <vector>

namespace scene_loader {
    class scene_loader; // the file this is defined in is not included to avoid circular inclusion
    
    class scene {
        friend class scene_loader;
        scene_loader* m_scene_loader; // this is private: the only reason it is ever used is to change the active scene, and subclasses should do that through change_scene()
    public:
        scene();
        virtual ~scene() = default;
        virtual void update(float delta) {}
        virtual void render() = 0;

        virtual void freeze() {}
        virtual void reheat() {}

        //this causes this object to be destroyed, unless "someone" takes ownership of it
        scene* change_scene(scene* s);
    };
};

#endif //GLSTUFF_SCENE_HPP
