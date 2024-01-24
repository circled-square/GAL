#ifndef GAL_TEXTURE_DEMO_HPP
#define GAL_TEXTURE_DEMO_HPP

#include "../../include/graphics/renderer/renderer.hpp"
#include "../../include/graphics/texture/texture.hpp"
#include "../scene_with_previous_scene/scene_with_previous_scene.hpp"
#include "../../include/graphics/image/image.hpp"

namespace scene_demos {
    class texture_demo : public scene_with_previous_scene {
        gal::graphics::renderer m_renderer;
        gal::graphics::vertex_array m_vao;
        gal::graphics::shader_program m_shader;
        gal::graphics::image m_img;
        gal::graphics::texture m_tex;
    public:
        texture_demo();
        void update(float delta) final;
        void render() final;
        void reheat() final;
    };
}

#endif //GAL_TEST_TEXTURE_HPP
