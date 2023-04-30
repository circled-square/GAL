#ifndef GLSTUFF_SCENE_DEMO_HPP
#define GLSTUFF_SCENE_DEMO_HPP

#include "scene_demo.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace scene_demos {

    class main_scene_demo : public scene_demo {
        glm::mat4 mvp_mat_1, mvp_mat_2;
        glm::mat4 model_mat_1, model_mat_2;
        glm::mat4 view_mat, proj_mat;

        // only accounts for position and rotation, not scaling
        static glm::mat4 make_model_matrix(glm::vec3 pos, glm::vec3 rotation) {
            using namespace glm;

            return translate(
                rotate(
                    mat(1.f),
                    rotation
                ),
                pos
            );
        }

        // only accounts for position and rotation, not scaling
        static glm::mat4 make_view_matrix(glm::vec3 pos, glm::vec3 rotation) {
            using namespace glm;

            return rotate(
                translate(
                    mat(1.f),
                    -pos
                ),
                -rotation
            );
        }

        static glm::mat4 make_proj_matrix(vec2 aspect_ratio) {
            //make aspect ratio have 1 as the smaller one
            aspect_ratio /= std::min(aspect_ratio.x, aspect_ratio.y);
                
            return glm::ortho(-aspect_ratio.x, aspect_ratio.x, -aspect_ratio.y, aspect_ratio.y, -1.f, 1.f);
        }

        gl::vertex_array vao;
        struct vertex_t {
            vec2 pos;
            f32 angle = 0.f; // this is not a uniform only for debug purposes
            vec2 tex_coord;
        };
        static const array<vertex_t, 4> vertex_data {
                vertex_t
                { { .5,  .5}, 0, {1., 1.} },
                { { .5, -.5}, 0, {1., 0.} },
                { {-.5, -.5}, 0, {0., 0.} },
                { {-.5,  .5}, 0, {0., 1.} }
        };
        static const array<uint, 3, 2> indices({
            array<uint, 3>{0, 1, 2},
            {2, 3, 0}
        });
        gl::shader_program shader(read_file("src/shader/vert.glsl"), read_file("src/shader/frag.glsl"));
        gl::renderer renderer;
        stb::image img;
        gl::texture tex;
    public:
        main_scene_demo() 
                : vao(gl::vertex_buffer(vertex_data), gl::index_buffer(indices)),
                shader(read_file("src/shader/vert.glsl"), read_file("src/shader/frag.glsl")),
                renderer(),
                img("src/resources/example.png"),
                tex(img.buffer, img.w, img.h, 4) {
            vao.specify_attribs<vec2, f32, vec2>();

            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBlendEquation(GL_FUNC_ADD);
            glEnable(GL_BLEND);

            //compute the mvp matrices
            mat4 model_mat_1 = make_model_matrix(vec3(0,0,0), vec3(0,0,1)); //translation, rotation, scale of the model?
            mat4 model_mat_2 = make_model_matrix(vec3(1,1,0), vec3(0,0,3)); //translation, rotation, scale of the model?
            mat4 view_mat = make_view_matrix(vec3(1,0,0), vec3(0,0,0));
            mat4 proj_mat = make_proj_matrix(vec2(4,3));

            mvp_mat_1 = proj_mat * view_mat * model_mat_1;
            mvp_mat_2 = proj_mat * view_mat * model_mat_2;

            int texture_slot = 0;
            tex.bind(texture_slot);
            shader.set_uniform<int>("u_texture_slot", texture_slot);
        }

        ~main_scene_demo() {}
    
        void update(float delta) override {
            //ImGui_ImplOpenGL3_NewFrame();
            //ImGui_ImplGlfw_NewFrame();
            //ImGui::NewFrame();
            //renderer.clear();

            {
                ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

                ImGui::Text("Application average %.1f FPS", imgui_io.Framerate);
                ImGui::End();
            }
            
            
            // edit the MVPs...
            model_mat_1 = rotate(model_mat_1, vec3(0, 0, delta * pi / 4);
            model_mat_2 = rotate(model_mat_2, vec3(0, 0, -delta * pi / 2);
            mvp_1 = proj_mat * view_mat * model_mat_1;
            mvp_2 = proj_mat * view_mat * model_mat_2;
            
            
            ///ImGui::Render();
            ///ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                
            ///window.swap_buffers();
            ///window.poll_events();
        }

        void render() override {
            shader.set_uniform("u_color", vec4(.8, .4, .4, 1.0));
            shader.set_uniform("u_mvp", mvp_mat_1);
            renderer.draw(vao, shader, 0, 2);
            
            shader.set_uniform("u_color", vec4(.4, .4, .8, .5));
            shader.set_uniform("u_mvp", mvp_mat_2);
            renderer.draw(vao, shader, 0, 2);
        }
    };
};

#endif //GLSTUFF_SCENE_DEMO_HPP
