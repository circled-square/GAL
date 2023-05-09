#ifndef GLSTUFF_MUL_OBJ_DRAW_DEMO_HPP
#define GLSTUFF_MUL_OBJ_DRAW_DEMO_HPP

#include "../gl/vertex_array.hpp"
#include "../gl/shader.hpp"
#include "../gl/texture.hpp"
#include "../gl/renderer.hpp"
#include "../stb/image.hpp"
#include "scene_with_previous_scene.hpp"

#include <glm/glm.hpp>

namespace scene_demos {

    class mul_obj_draw_demo : public scene_with_previous_scene {
        gl::vertex_array m_vao;
        gl::shader_program m_shader;
        gl::renderer m_renderer;
        stb::image m_img;
        gl::texture m_tex;
        glm::mat4 m_model_mat_1, m_model_mat_2;
        glm::mat4 m_view_mat, m_proj_mat;
        glm::mat4 m_mvp_mat_1, m_mvp_mat_2;

        static glm::mat4 make_model_matrix(glm::vec3 pos, float rotation, float scale_factor);// only accounts for position and z-rotation
        static glm::mat4 make_view_matrix(glm::vec3 pos, float rotation);// only accounts for position and z-rotation, not scaling
        static glm::mat4 make_proj_matrix(glm::vec2 aspect_ratio);
    public:
        mul_obj_draw_demo();

        void update(float delta) final;

        void render() final;

        void reheat() final;
    };
};

#endif //GLSTUFF_MUL_OBJ_DRAW_DEMO_HPP
