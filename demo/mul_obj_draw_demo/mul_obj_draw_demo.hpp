#ifndef GAL_MUL_OBJ_DRAW_DEMO_HPP
#define GAL_MUL_OBJ_DRAW_DEMO_HPP

#include "../../include/graphics/vertex_array/vertex_array.hpp"
#include "../../include/graphics/shader/shader.hpp"
#include "../../include/graphics/texture/texture.hpp"
#include "../../include/graphics/renderer/renderer.hpp"
#include "../../include/graphics/image/image.hpp"
#include "../scene_with_previous_scene/scene_with_previous_scene.hpp"

#include <glm/glm.hpp>

namespace scene_demos {

    class mul_obj_draw_demo : public scene_with_previous_scene {
        gal::graphics::vertex_array m_vao;
        gal::graphics::shader_program m_shader;
        gal::graphics::renderer m_renderer;
        gal::graphics::image m_img;
        gal::graphics::texture m_tex;
        glm::mat4 m_model_mat_1, m_model_mat_2;
        glm::mat4 m_view_mat, m_proj_mat;
        glm::mat4 m_mvp_mat_1, m_mvp_mat_2;
    public:
        mul_obj_draw_demo();

        void update(float delta) final;

        void render() final;

        void reheat() final;
    };
};

#endif //GAL_MUL_OBJ_DRAW_DEMO_HPP
