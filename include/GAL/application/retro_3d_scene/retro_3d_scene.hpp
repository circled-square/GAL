#ifndef GAL_APPLICATION_RETRO_3D_SCENE_HPP
#define GAL_APPLICATION_RETRO_3D_SCENE_HPP

#include "../scene.hpp"
#include "../../graphics/renderer/retro_renderer.hpp"
#include "../../graphics/framebuffer.hpp"
#include <glm/glm.hpp>
#include <scluk/exception.hpp>
#include <vector>
#include <optional>
#include "node.hpp"

namespace gal::application::retro {
    struct named_texture {
        std::string name;
        graphics::texture texture;
    };
    struct shader_capabilities {
        //a vector of named textures; the name will be used to retreive the position of the uniform for its slot
        std::vector<named_texture> textures;
    };

    class scene : public gal::application::scene {
        graphics::retro::renderer m_renderer;
        node m_root;
        static void render_node(scene &s, const node &n, glm::mat4 transform);

        //post processing through multipass rendering
        graphics::renderer m_post_processing_renderer;
        graphics::vertex_array m_whole_screen_vao;
    protected:
        std::vector<graphics::framebuffer> postfx_fbos;
        std::vector<graphics::shader_program> postfx_shaders;
        std::vector<shader_capabilities> postfx_shader_capabilities;
    public:
        virtual ~scene() = default;
        scene();

        virtual void render(glm::ivec2 resolution) final;
        virtual void reheat() final;

        graphics::retro::renderer& get_renderer();

        virtual graphics::retro::camera& get_camera() = 0;
        node& get_root();
        node& get_node(std::string_view path);
    };
}

#endif // GAL_APPLICATION_RETRO_3D_SCENE_HPP
