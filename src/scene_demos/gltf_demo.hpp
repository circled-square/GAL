#ifndef GLSTUFF_GLTF_DEMO_HPP
#define GLSTUFF_GLTF_DEMO_HPP

#include "../gl/renderer.hpp"
#include "../gl/vertex_array.hpp"
#include "../gl/texture.hpp"
#include "../gl/shader.hpp"
#include "scene_with_previous_scene.hpp"
#include <tinygltf/tiny_gltf.h>

namespace scene_demos {

    struct model_t { 
        gl::vertex_array vao;
        gl::texture texture;
        gl::shader_program shader;
    };
    class gltf_demo : public scene_with_previous_scene {
        gl::renderer m_renderer;

        model_t m_model;
        glm::mat4 m_mvp;
        glm::mat4 m_model_mat, m_view_mat, m_proj_mat;
    public:
        gltf_demo();
        void update(float delta) final;
        void render() final;
        void reheat() final;
    };

} // scene_demos

#endif //GLSTUFF_GLTF_DEMO_HPP
