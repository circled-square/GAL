#ifndef GAL_MENU_DEMO_HPP
#define GAL_MENU_DEMO_HPP

#include "../../include/application/scene.hpp"
#include "../../include/graphics/renderer/renderer.hpp"
#include "../scene_with_previous_scene/scene_with_previous_scene.hpp"
#include <scluk/exception.hpp>

#include <vector>
#include <string>
#include <memory>

namespace scene_demos {

    struct named_scene {
        using scene_ctor_t = scene_with_previous_scene*(*)();

        scene_ctor_t  scene_constructor;
        std::string name;
    };

    //this class owns the pointers to the scenes that it leads to
    class menu_demo : public gal::application::scene {
        gal::graphics::renderer m_renderer;

        std::vector<named_scene> m_scene_vector;
        std::unique_ptr<scene_with_previous_scene> m_current_scene;
    public:
        menu_demo(std::vector<named_scene> scene_vector);

        ~menu_demo() {}
    
        virtual void update(float delta) override;

        virtual void render() override;

        virtual void reheat() override;

        void add_scene(named_scene s);
        void add_scene(named_scene::scene_ctor_t scene_ctor, std::string name);
        template<class T> inline
        void add_scene(std::string name) {
            add_scene({ []() -> scene_with_previous_scene* { return new T; }, std::move(name) });
        }
        void activate(const char* scene_name);

        menu_demo();
    };
};

#endif // GAL_MENU_DEMO_HPP
