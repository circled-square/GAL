#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
#include "stb/image.hpp"
#include <scluk/language_extension.hpp>
#include <scluk/exception.hpp>
#include <scluk/array.hpp>
#include <scluk/read_file.hpp>

#include "glfw/window.hpp"
#include "stb/image.hpp"
#include "gl/error_handling.hpp"
#include "gl/vertex_array.hpp"
#include "gl/shader.hpp"
#include "gl/renderer.hpp"
#include "gl/texture.hpp"

using namespace scluk;
using namespace glm;

int main() try {
    glfw::window window({800, 600}, "Hello OpenGL");

    if(glewInit() != GLEW_OK)
        throw scluk::runtime_error("GLEW failed to initialize!");

    gl::initialize_error_handling();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    glEnable(GL_BLEND);

    out("%", glGetString(GL_VERSION));

    vec3 model_position = vec3(0,1,0);
    mat4 model_mat = glm::translate(mat4(1.0f), model_position); //translation, rotation, scale of the model?
    vec3 cam_position = vec3(-1,0,0);
    mat4 view_mat = glm::translate(mat4(1.0f), -cam_position); // translation, rotation, scale of the camera
    mat4 proj_mat = glm::ortho(-2.f, 2.f, -1.5f, 1.5f, -1.f, 1.f); // aspect ratio dependant coordinates -> normalized device coordinates
    mat4 mvp_mat = proj_mat * view_mat * model_mat;

    struct vertex_t {
        vec2 pos;
        f32 angle = 0.f; // this is not a uniform only for debug purposes
        vec2 tex_coord;
    };
    array<vertex_t, 4> vertex_data {
            vertex_t
            { { .5,  .5}, 0, {1., 1.} },
            { { .5, -.5}, 0, {1., 0.} },
            { {-.5, -.5}, 0, {0., 0.} },
            { {-.5,  .5}, 0, {0., 1.} }
    };
    array<uint, 3, 2> indices({
        array<uint, 3>{0, 1, 2},
        {2, 3, 0}
    });

    gl::vertex_array vao{gl::vertex_buffer(vertex_data), gl::index_buffer(indices)};
    vao.specify_attribs<vec2, f32, vec2>();

    gl::shader_program shader(read_file("src/shader/vert.glsl"), read_file("src/shader/frag.glsl"));

    gl::renderer renderer;

    stb::image img("src/resources/example.png");
    gl::texture tex(img.buffer, img.w, img.h, 4);
    tex.bind();

    shader.set_uniform<int>("u_texture_slot", 0);
    shader.set_uniform("u_mvp", mvp_mat);


    bool keep_going = true;
    window.set_key_cb([&](glfw::window_t, int k, int, int, int) {
        keep_going = keep_going && k != GLFW_KEY_ESCAPE;
    });
    auto start = std::chrono::high_resolution_clock::now();

    while (!window.should_close() && keep_going) {
        const f32 seconds = std::chrono::duration<f32, std::ratio<1>>(std::chrono::high_resolution_clock::now()- start).count();

        //rotate the vertex_data (this of course could just be a uniform but i wanted to experiment with vao.specify_attribs<...>()
        for (auto& v: vertex_data)
            v.angle = pi<f32>() * sin(seconds / 2.0f);

        vao.vbo.update(vertex_data);

        renderer.clear();

        shader.set_uniform("u_color", vec4(.8, .4, .4, 1.0));
        renderer.draw(vao, shader, 0, 1);

        shader.set_uniform("u_color", vec4(.4, .4, .8, 1.0));
        renderer.draw(vao, shader, 1, 1);

        window.swap_buffers();
        window.poll_events();
    }

} catch (std::exception& e) {
    scluk::out("Caught exception!\n %", e.what());
}


