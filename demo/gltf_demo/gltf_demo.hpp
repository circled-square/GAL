#ifndef GAL_GLTF_DEMO_HPP
#define GAL_GLTF_DEMO_HPP

#include "../../include/graphics/renderer/renderer.hpp"
#include "../../include/graphics/vertex_array/vertex_array.hpp"
#include "../../include/graphics/texture/texture.hpp"
#include "../../include/graphics/shader/shader.hpp"
#include "../scene_with_previous_scene/scene_with_previous_scene.hpp"

namespace scene_demos {
    struct mvp_t{
        glm::mat4 model, view, proj, mvp;
    };

    struct model_t {
        gal::graphics::vertex_array vao;
        gal::graphics::texture texture;
        gal::graphics::shader_program shader;
    };
    class gltf_demo : public scene_with_previous_scene {
        gal::graphics::renderer m_renderer;

        model_t m_model;
        mvp_t m_mvp;
    public:
        gltf_demo();
        void update(float delta) final;
        void render() final;
        void reheat() final;
    };

} // scene_demos

#endif //GAL_GLTF_DEMO_HPP
