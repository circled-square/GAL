#include <GAL/application/retro_3d_scene/retro_3d_scene.hpp>
#include <scluk/log.hpp>

namespace gal::application::retro {

    struct postfx_vertex_t {
        glm::vec2 pos;
        using layout_t = decltype(gal::graphics::static_vertex_layout(pos));
    };

    //coords of big triangle covering the whole screen
    static const std::array<postfx_vertex_t, 3> whole_screen_vertices {
        postfx_vertex_t
        {{-1,-1}}, {{3,-1}}, {{-1,3}}
    };

    static const std::array<glm::uvec3, 1> whole_screen_indices = {
        glm::uvec3 {0, 1, 2}
    };

    static gal::graphics::vertex_array make_whole_screen_vao() {
        gal::graphics::vertex_buffer vbo(whole_screen_vertices);
        gal::graphics::index_buffer ibo(whole_screen_indices);

        return gal::graphics::vertex_array(std::move(vbo), std::move(ibo), postfx_vertex_t::layout_t::to_vertex_layout());
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


    scene::scene()
        : m_renderer(),
        m_root(""),
        m_whole_screen_vao(make_whole_screen_vao())
    {}


    void scene::render(glm::ivec2 resolution) {
        assert(postfx_shaders.size() == postfx_fbos.size() && postfx_shaders.size() == postfx_shader_capabilities.size());
        // 3d rendering
        {
            glm::ivec2 render_res;
            //if there is post processing to be done bind the correct fbo and use its res, otherwise use window res
            if(!postfx_fbos.empty()) {
                postfx_fbos[0].bind_draw();
                render_res = postfx_fbos[0].get_texture().resolution();
            } else {
                graphics::framebuffer::unbind(); // bind the default framebuffer on the last draw call
                render_res = resolution;
            }

            m_renderer.clear();

            glViewport(0, 0, render_res.x, render_res.y);
            get_camera().set_aspect_ratio(render_res);
            scene::render_node(*this, get_root(), glm::mat4(1));
        }

        // post processing
        for(size_t i = 0; i < postfx_shaders.size(); i++) {
            glm::ivec2 render_res;
            //bind the next fbo for drawing, or the default fbo on the last pass, and set the correct render_res for glViewport
            if(i+1 == postfx_fbos.size()) {
                graphics::framebuffer::unbind(); // bind the default framebuffer on the last draw call
                render_res = resolution;
            } else {
                postfx_fbos[i+1].bind_draw();
                render_res = postfx_fbos[i+1].get_texture().resolution();
            }
            glViewport(0,0,render_res.x, render_res.y);

            //post processing
            int next_texture_slot = 0;
            graphics::framebuffer& src_fbo = postfx_fbos[i];
            graphics::texture& src_tex = src_fbo.get_texture();
            glm::vec2 src_res = src_tex.resolution();
            graphics::shader_program& shader = postfx_shaders[i];
            if (!postfx_shader_capabilities[i].textures.empty()) {
                for(auto& [name, texture] : postfx_shader_capabilities[i].textures) {
                    texture.bind(next_texture_slot);
                    shader.set_uniform<int>(name.c_str(), next_texture_slot);
                    next_texture_slot++;
                }
            }

            src_tex.bind(next_texture_slot);
            shader.set_uniform<int>("u_texture_slot", next_texture_slot);
            shader.set_uniform<glm::vec2>("u_src_res", src_res);
            next_texture_slot++;

            // see red background? bad thing
            m_post_processing_renderer.clear(glm::vec4(1,0,0,1));
            // draw
            m_post_processing_renderer.draw(m_whole_screen_vao, shader);
        }
    }


    graphics::retro::renderer& scene::get_renderer() { return m_renderer; }

    void scene::reheat() {
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
