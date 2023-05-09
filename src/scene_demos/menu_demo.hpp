#ifndef GLSTUFF_MENU_DEMO_HPP
#define GLSTUFF_MENU_DEMO_HPP

#include "../scene_loader/scene.hpp"
#include "../gl/renderer.hpp"
#include "../scene_loader/scene_loader.hpp"

#include <vector>
#include <string>
#include <memory>

namespace scene_demos {

    struct named_scene {
        std::unique_ptr<scene_loader::scene> scene;
        std::string name;
    };

    //this class owns the pointers to the scenes that it leads to
    class menu_demo : public scene_loader::scene {
        gl::renderer m_renderer;

        std::vector<named_scene> m_scene_vector;
    public:
        menu_demo(std::vector<named_scene> scene_vector);

        ~menu_demo() {}
    
        void update(float delta) override;

        void render() override;
    };
};

#endif // GLSTUFF_MENU_DEMO_HPP
