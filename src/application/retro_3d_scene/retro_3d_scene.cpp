#include <GAL/application/retro_3d_scene/retro_3d_scene.hpp>
#include <scluk/log.hpp>

namespace gal::application::retro {

    struct post_process_vertex_t {
        glm::vec2 pos;
        using layout_t = decltype(gal::graphics::static_vertex_layout(pos));
    };

    //coords of big triangle covering the whole screen
    static const std::array<post_process_vertex_t, 3> whole_screen_vertices {
        post_process_vertex_t
        {{-1,-1}}, {{3,-1}}, {{-1,3}}
    };

    static graphics::vertex_array make_whole_screen_vao() {
        std::vector<graphics::vertex_buffer> vbos;
        std::vector<graphics::index_buffer> ibos;
        vbos.emplace_back(whole_screen_vertices);

        return graphics::vertex_array(std::move(vbos), std::move(ibos));
    }

    // static method
    void scene::render_node(scene &s, const node &n, glm::mat4 transform) {
        transform = transform * n.transform;

        if (n.renderable) {
            n.renderable->set_model_mat(transform);
            s.get_renderer().draw(*n.renderable, s.get_camera());
        }

        for(const auto& [name, child] : n.children())
            scene::render_node(s, child, transform);
    }

    scene::scene() : scene({}, {}) {}

    scene::scene(std::vector<graphics::framebuffer> multipass_fbo_vec, std::vector<graphics::shader_program> multipass_shader_vec)
        : m_renderer(),
        m_root(""),
        m_multipass_fbo_vec(std::move(multipass_fbo_vec)),
        m_multipass_shader_vec(std::move(multipass_shader_vec)),
        m_whole_screen_vao(make_whole_screen_vao())
    {
        assert(m_multipass_fbo_vec.size() == m_multipass_shader_vec.size());
    }

    void scene::render(glm::ivec2 resolution) {
        m_renderer.clear();
        get_camera().set_aspect_ratio(resolution); // TODO: the aspect ratio is only correct if we draw to FBOs with the same aspect ratio as the original window

        if(!m_multipass_fbo_vec.empty())
            m_multipass_fbo_vec[0].bind_draw();

        scene::render_node(*this, get_root(), glm::mat4(1));

        // successive passes for post-processing
        for(size_t i = 0; i < m_multipass_shader_vec.size(); i++) {
            //bind the next framebuffer, or the default framebuffer on the last call
            if(i+1 >= m_multipass_fbo_vec.size()) {
                graphics::framebuffer::unbind(); // bind the default framebuffer on the last draw call
            } else {
                m_multipass_fbo_vec[i+1].bind_draw();
            }

            graphics::texture& tex = m_multipass_fbo_vec[i].get_texture();
            const int texture_slot = 0;
            tex.bind(texture_slot);
            graphics::shader_program& shader = m_multipass_shader_vec[i];
            shader.bind();
            shader.set_uniform("u_texture_slot", texture_slot);
            m_post_processing_renderer.draw_without_indices(m_whole_screen_vao, shader, 0, whole_screen_vertices.size());
        }
    }

    graphics::retro::renderer& scene::get_renderer() { return m_renderer; }

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

    node& scene::get_root() { return m_root; }

    node& scene::get_node(std::string_view path) {
        if(path[0] != '/')
            throw scluk::runtime_error("the first character of a path passed to scene::get_from_path must be '/'; instead path = \"{}\"", path);

        std::string_view subpath(path.begin()+1, path.end());
        return m_root.get_from_path(subpath);
    }
} // namespace gal::application::retro
