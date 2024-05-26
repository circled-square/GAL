#ifndef GAL_APPLICATION_RETRO_3D_SCENE_HPP
#define GAL_APPLICATION_RETRO_3D_SCENE_HPP

#include "../scene.hpp"
#include "../../graphics/renderer/retro_renderer.hpp"
#include <glm/glm.hpp>
#include <scluk/exception.hpp>
#include <vector>
#include "node.hpp"

namespace gal::application::retro {
    class scene : public gal::application::scene {
        graphics::retro::renderer m_renderer;
        node m_root;
        static void render_node(scene &s, const node &n, glm::mat4 transform);

        //post processing through multipass rendering
        std::vector<graphics::framebuffer> m_multipass_fbo_vec;
        std::vector<graphics::shader_program> m_multipass_shader_vec;
        graphics::renderer m_post_processing_renderer;
        graphics::vertex_array m_whole_screen_vao;

    public:
        virtual ~scene() = default;
        scene();
        scene(std::vector<graphics::framebuffer> multipass_fbo_vec, std::vector<graphics::shader_program> multipass_shader_vec);

        virtual void render(glm::ivec2 resolution) final;
        virtual void reheat() final;

        graphics::retro::renderer& get_renderer();

        virtual graphics::retro::camera& get_camera() = 0;
        node& get_root();
        node& get_node(std::string_view path);
    };
}

#endif // GAL_APPLICATION_RETRO_3D_SCENE_HPP
