#ifndef GLSTUFF_GLTF_DEMO_HPP
#define GLSTUFF_GLTF_DEMO_HPP

#include "../gl/renderer.hpp"
#include "scene_with_previous_scene.hpp"
#include <tinygltf/tiny_gltf.h>

namespace scene_demos {

    class gltf_demo : public scene_with_previous_scene {
        gl::renderer m_renderer;
        //gl::vertex_array m_vao;
        //gl::shader_program m_shader;
        tinygltf::Model m_model;
    public:
        gltf_demo();
        void update(float delta) final;
        void render() final;
        void reheat() final;
    };

} // scene_demos

#endif //GLSTUFF_GLTF_DEMO_HPP
