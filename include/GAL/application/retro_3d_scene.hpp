#ifndef RETRO_3D_SCENE_HPP
#define RETRO_3D_SCENE_HPP

#include "scene.hpp"
#include "../graphics/renderer/retro_renderer.hpp"
#include <memory>
#include <glm/glm.hpp>
#include <map>
#include <scluk/exception.hpp>

namespace gal::application {
    namespace retro {
        class node {
            std::map<std::string, node> m_children; // consider changing this to a vec<(str, node)>, hashmap<str, node> or hashmap<str, unique_ptr<node>>
            node* m_father;
            std::string m_name;
        public:
            glm::mat4 transform;
            std::shared_ptr<gal::graphics::retro::simple_renderable> renderable;

            node(std::string name, std::shared_ptr<gal::graphics::retro::simple_renderable> renderable = nullptr, glm::mat4 transform = glm::mat4(1));
            node(node&) = delete;
            node(node&& n);

            void add_child(node c);
            void emplace_child(auto...args) {
                add_child(node(std::move(args)...));
            }

            node& get(const std::string& name);
            node* try_get(const std::string& name);
            node& father();
            node* try_get_father();

            const std::map<std::string, node>& children() const;

            std::string absolute_node_path() const;
            const std::string& name() const;

            node& get_from_path(std::string_view path) {
                std::string_view subpath = path;
                node* current_node = this;
                while(true) {
                    size_t separator_position = subpath.find('/');
                    if (separator_position == std::string_view::npos) {
                        //could not find separator; base case
                        return current_node->get(std::string(subpath));
                    } else {
                        std::string_view next_step(subpath.data(), subpath.data() + separator_position);
                        current_node = &current_node->get(std::string(next_step));
                        subpath.remove_prefix(separator_position + 1);
                    }
                }
            }
        };

        class scene : public gal::application::scene {
            gal::graphics::retro::renderer m_renderer;
            node m_root;
        public:
            virtual ~scene() = default;
            scene();

            virtual void render(glm::ivec2 resolution) final;
            virtual void reheat() final;

            gal::graphics::retro::renderer& get_renderer();

            virtual gal::graphics::retro::camera& get_camera() = 0;
            node& get_root();
            node& get_node(std::string_view path) {
                if(path[0] != '/')
                    throw scluk::runtime_error("the first character of a path passed to scene::get_from_path must be '/'; instead path = \"{}\"", path);

                std::string_view subpath(path.begin()+1, path.end());
                return m_root.get_from_path(subpath);
            }
        };
    }
}

#endif // RETRO_3D_SCENE_HPP
