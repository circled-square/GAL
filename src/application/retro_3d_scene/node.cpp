#include <GAL/application/retro_3d_scene/node.hpp>
#include <scluk/exception.hpp>

namespace gal::application::retro {
    node::node(node&& n)
        : m_children(std::move(n.m_children)),
        m_father(n.m_father),
        m_name(std::move(n.m_name)),
        transform(n.transform),
        renderable(std::move(n.renderable))
    {
        n.m_father = nullptr;
        // fix children's father pointers
        for (auto& [name, child] : m_children)
            child.m_father = this;
    }

    node::node(std::string name, std::shared_ptr<gal::graphics::retro::retro_renderable> renderable, glm::mat4 transform)
        : m_father(nullptr),
        m_name(std::move(name)),
        transform(transform),
        renderable(std::move(renderable))
    {
        for (char& c : m_name)
            if (c == '/')
                c = '\\';
    }


    void node::add_child(node c) {
        c.m_father = this;
        std::string k = c.name();
        m_children.insert({k, std::move(c)});
    }





    node* node::try_get_child(const std::string& name) {
        auto it = m_children.find(name);

        if (it != m_children.end()) {
            return &it->second;
        } else {
            return nullptr;
        }
    }
    node& node::get_child(const std::string& name) {
        node* c = try_get_child(name);

        if (c) {
            return *c;
        } else {
            throw scluk::runtime_error("node at path '{}' has no child named '{}'", absolute_node_path(), name);
        }
    }

    node *node::try_get_child(int n) {
        auto it = m_children.begin();
        for(int i = 0; i < n; n++) {
            if (it == m_children.end()) {
                return nullptr;
            }
            it++;
        }
        if (it == m_children.end()) {
            return nullptr;
        }
        return &it->second;
    }
    node &node::get_child(int n) {
        node* c = try_get_child(n);

        if (c) {
            return *c;
        } else {
            throw scluk::runtime_error("node at path '{}' has no {}th child", absolute_node_path(), n);
        }
    }

    node* node::try_get_father() {
        return m_father;
    }
    node& node::father() {
        if(m_father)
            return *m_father;
        else
            throw scluk::runtime_error("node with name '' has no father");
    }
    const std::map<std::string, node> &node::children() const { return m_children; }

    std::string node::absolute_node_path() const {
        return m_father ? m_father->absolute_node_path() + "/" + m_name : m_name;
    }
    const std::string& node::name() const { return m_name; }

    node& node::get_from_path(std::string_view path) {
        std::string_view subpath = path;
        node* current_node = this;
        while(true) {
            size_t separator_position = subpath.find('/');
            if (separator_position == std::string_view::npos) {
                //could not find separator; base case
                return current_node->get_child(std::string(subpath));
            } else {
                std::string_view next_step(subpath.data(), subpath.data() + separator_position);
                current_node = &current_node->get_child(std::string(next_step));
                subpath.remove_prefix(separator_position + 1);
            }
        }
    }
}
