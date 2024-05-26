#ifndef GAL_APPLICATION_RETRO_3D_SCENE_NODE_HPP
#define GAL_APPLICATION_RETRO_3D_SCENE_NODE_HPP

#include <map>
#include <string>
#include <memory>
#include "../../graphics/renderer/retro_renderer.hpp"

namespace gal::application::retro {
 class node {
        std::map<std::string, node> m_children; // consider changing this to a vec<(str, node)>, hashmap<str, node> or hashmap<str, unique_ptr<node>>
        node* m_father;
        std::string m_name;
    public:
        glm::mat4 transform;
        std::shared_ptr<gal::graphics::retro::retro_renderable> renderable;

        node(std::string name, std::shared_ptr<gal::graphics::retro::retro_renderable> renderable = nullptr, glm::mat4 transform = glm::mat4(1));
        node(node&) = delete;
        node(node&& n);

        void add_child(node c);
        void emplace_child(auto...args) {
            add_child(node(std::move(args)...));
        }

        node& get_child(const std::string& name);
        node* try_get_child(const std::string& name);
        node& get_child(int n);
        node* try_get_child(int n);

        node& father();
        node* try_get_father();

        const std::map<std::string, node>& children() const;

        std::string absolute_node_path() const;
        const std::string& name() const;

        node& get_from_path(std::string_view path);
    };
}

#endif // GAL_APPLICATION_RETRO_3D_SCENE_NODE_HPP
