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

    array<vec2, 4> positions;
    array<uint, 3, 2> indices({
        array<uint, 3>{0, 1, 2},
        {2, 3, 0}
    });

    gl::vertex_array vao{gl::vertex_buffer(positions), gl::index_buffer(indices)};


    vao.specify_attribs<vec2>();

    gl::shader_program shader(read_file("shader/vert.glsl"), read_file("shader/frag.glsl"));
    shader.set_uniform("u_color", vec4(.8, .4, .4, 1.0));

    bool keep_going = true;
    window.set_key_cb([&](glfw::window_t, int k, int, int, int) { 
        keep_going = keep_going && k != GLFW_KEY_ESCAPE;
    });
    

    auto start = std::chrono::high_resolution_clock::now();

    while (!window.should_close() && keep_going) {
        const f32 seconds = std::chrono::duration<f32, std::ratio<1>>(std::chrono::high_resolution_clock::now()- start).count();

        //rotate the vertices
        for (int i: index(positions)) {
            const f32 theta = seconds + pi / 2.0f * i;

            const f32 mod = 1.0f/sqrt(2.0f);
            auto &p = positions[i];

            p.x = mod * std::sin(theta);
            p.y = mod * std::cos(theta);
        }

        vao.vbo.update(positions);

        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        vao.bind();
        shader.set_uniform("u_color", vec4(.4, .4, .8, 1.0));
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (const void*)(3*sizeof(unsigned int))); //draw triangles({1}), read {2} elements of type {3} starting from the {4}th byte from the bound GL_ELEMENT_ARRAY_BUFFER
        shader.set_uniform("u_color", vec4(.8, .4, .4, 1.0));
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (const void*)0); //draw triangles({1}), read {2} elements of type {3} starting from the {4}th byte from the bound GL_ELEMENT_ARRAY_BUFFER
        vao.unbind();

        window.swap_buffers();
        window.poll_events();
    }

} catch (std::exception& e) {
    scluk::out("Caught exception!\n %", e.what());
}


