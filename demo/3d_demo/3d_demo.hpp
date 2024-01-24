#ifndef GAL_3D_DEMO_HPP
#define GAL_3D_DEMO_HPP

#include "../../include/graphics/vertex_array/vertex_array.hpp"
#include "../../include/graphics/shader/shader.hpp"
#include "../../include/graphics/texture/texture.hpp"
#include "../../include/graphics/renderer/renderer.hpp"
#include "../../include/graphics/image/image.hpp"
#include "../scene_with_previous_scene/scene_with_previous_scene.hpp"

#include <glm/glm.hpp>

namespace scene_demos {

    class three_dimensional_demo : public scene_with_previous_scene {
        gal::graphics::vertex_array m_vao;
        gal::graphics::shader_program m_shader;
        gal::graphics::renderer m_renderer;
        gal::graphics::image m_img;
        gal::graphics::texture m_tex;
        glm::mat4 m_model_mat, m_view_mat, m_proj_mat, m_mvp_mat;

        static glm::mat4 make_model_matrix(glm::vec3 pos, float rotation, float scale_factor);// only accounts for position and z-rotation
        static glm::mat4 make_view_matrix(glm::vec3 pos, float rotation);// only accounts for position and z-rotation, not scaling
        static glm::mat4 make_proj_matrix(glm::vec2 aspect_ratio);
    public:
        three_dimensional_demo();

        void update(float delta) final;

        void render() final;

        void reheat() final;
    };
};

#endif //GAL_3D_DEMO_HPP
