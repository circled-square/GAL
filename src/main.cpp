#include <GL/glew.h>
#include <scluk/language_extension.hpp>
#include <scluk/exception.hpp>
#include <scluk/array.hpp>
#include <scluk/read_file.hpp>
#include <glm/glm.hpp>
#include "glfw/window.hpp"
#include "gl/error_handling.hpp"
#include "gl/vertex_array.hpp"
#include "gl/shader.hpp"
#include "gl/renderer.hpp"
#include <chrono>

using namespace scluk;
using namespace glm;
using std::numbers::pi;
int main() try {
    glfw::window window({640, 640}, "Hello OpenGL");

    if(glewInit() != GLEW_OK)
        throw scluk::runtime_error("GLEW failed to initialize!");

    gl::initialize_error_handling();

    out("%", glGetString(GL_VERSION));

    struct vertex_t {
        vec2 pos;
        float angle = 0.f; // this is not a uniform only for debug purposes
    };
    array<vertex_t, 4> vertex_data {
            vertex_t { { .5, .5} },
            { { .5, -.5} },
            { {-.5, -.5} },
            { {-.5,  .5} }
    };
    array<uint, 3, 2> indices({
        array<uint, 3>{0, 1, 2},
        {2, 3, 0}
    });

    gl::vertex_array vao{gl::vertex_buffer(vertex_data), gl::index_buffer(indices)};


    vao.specify_attribs<vec2, float>();

    gl::shader_program shader(read_file("shader/vert.glsl"), read_file("shader/frag.glsl"));

    gl::renderer renderer;

    bool keep_going = true;
    window.set_key_cb([&](glfw::window_t, int k, int, int, int) { 
        keep_going = keep_going && k != GLFW_KEY_ESCAPE;
    });
    

    auto start = std::chrono::high_resolution_clock::now();

    while (!window.should_close() && keep_going) {
        const f32 seconds = std::chrono::duration<f32, std::ratio<1>>(std::chrono::high_resolution_clock::now()- start).count();

        //rotate the vertex_data (this of course could just be a uniform but i wanted to experiment with vao.specify_attribs<...>()
        for (auto& v: vertex_data)
            v.angle = pi * sin(seconds / 2.0f);

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


