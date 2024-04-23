#include <GAL/application/retro_3d_scene.hpp>
#include <scluk/log.hpp>

namespace gal::application::retro_3d {
    static void render_node(scene &s, const node &n, glm::mat4 transform) {
        transform = transform * n.transform;

        if (n.renderable) {
            n.renderable->get_model_mat() = transform;
            s.get_renderer().draw(*n.renderable, s.get_camera());
        }

        for(const auto& [name, child] : n.children())
            render_node(s, child, transform);
    }

    scene::scene() : m_renderer(gal::graphics::make_retro_renderer()), m_root("") {}

    void scene::render(glm::ivec2 resolution) {
        m_renderer.clear();
        get_camera().set_aspect_ratio(resolution);
        render_node(*this, get_root(), glm::mat4(1));
    }

    gal::graphics::single_shader_renderer& scene::get_renderer() { return m_renderer; }

    void scene::reheat() {
        m_renderer.set_clear_color(glm::vec4(0,0,0,1));

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendEquation(GL_FUNC_ADD);
        glEnable(GL_BLEND);

        // Enable depth test
        glEnable(GL_DEPTH_TEST);
        // Accept fragment if it closer to the camera than the former one
        glDepthFunc(GL_LESS);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }




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

    void node::add_child(node c) {
        c.m_father = this;
        std::string k = c.name().data();
        m_children.insert({k, std::move(c)});
    }

    node::node(std::string name, std::shared_ptr<gal::graphics::simple_renderable> renderable, glm::mat4 transform)
        : m_father(nullptr),
          m_name(name), 
          transform(transform), 
          renderable(std::move(renderable))
    {}



    node* node::try_get(const std::string& name) {
        auto it = m_children.find(name);

        if (it != m_children.end()) {
            return &it->second;
        } else {
            return nullptr;
        }
    }
    node& node::get(const std::string& name) {
        node* c = try_get(name);

        if (c) {
            return *c;
        } else {
            throw scluk::runtime_error("node at path '{}' has no child named '{}'", absolute_node_path(), name);
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
    node &scene::get_root() { return m_root; }
} // namespace gal::application::retro_3d
