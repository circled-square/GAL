#ifndef GLSTUFF_TEXTURE_DEMO_HPP
#define GLSTUFF_TEXTURE_DEMO_HPP

#include "../../include/graphics/renderer/renderer.hpp"
#include "../../include/graphics/texture/texture.hpp"
#include "../scene_with_previous_scene/scene_with_previous_scene.hpp"
#include "../../include/graphics/image/image.hpp"

namespace scene_demos {
    class texture_demo : public scene_with_previous_scene {
        gl::renderer m_renderer;
        gl::vertex_array m_vao;
        gl::shader_program m_shader;
        stb::image m_img;
        gl::texture m_tex;
    public:
        texture_demo();
        void update(float delta) final;
        void render() final;
        void reheat() final;
    };
} // scene_demos

#endif //GLSTUFF_TEST_TEXTURE_HPP
