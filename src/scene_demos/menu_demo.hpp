#ifndef GLSTUFF_MENU_DEMO_HPP
#define GLSTUFF_MENU_DEMO_HPP

#include "../application/scene.hpp"
#include "../gl/renderer.hpp"
#include "scene_with_previous_scene.hpp"
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
    class menu_demo : public application::scene {
        gl::renderer m_renderer;

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
        template<class T>
        void add_scene(std::string name) {
            add_scene({ []() -> scene_with_previous_scene* { return new T; }, name });
        }
        void activate(const char* scene_name) {
            for(named_scene& s : m_scene_vector) {
                if(s.name == scene_name) {
                    m_current_scene.reset(s.scene_constructor());
                    this->change_scene(m_current_scene.get());
                    m_current_scene->set_prev_scene(this);
                    return;
                }
            }
            throw scluk::runtime_error("menu_demo::activate called with string '%'; no such named scene", scene_name);
        }

        menu_demo();
    };
};

#endif // GLSTUFF_MENU_DEMO_HPP
