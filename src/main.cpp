#include <GL/glew.h>
#include <scluk/language_extension.hpp>
#include <scluk/exception.hpp>
#include <scluk/array.hpp>
#include <scluk/read_file.hpp>
#include <glm/glm.hpp>
#include "glfw/window.hpp"
#include "gl/shader.hpp"
#include "gl/error_handling.hpp"
#include <chrono>

using namespace scluk;
using namespace glm;
using std::numbers::pi;
int main() try {
    glfw::window window({640, 640}, "Hello World");

    if(glewInit() != GLEW_OK)
        throw scluk::runtime_error("GLEW failed to initialize!");

    gl::initialize_error_handling();

    out("%", glGetString(GL_VERSION));

    array<vec2, 4> positions;
    array<uint, 3, 2> indices({
        array<uint, 3>{0, 1, 2},
        {2, 3, 0}
    });

    uint vao; //vertex array object: associates a vertex buffer with the layout of its vertices
    glCreateVertexArrays(1, &vao);

    uint vbo;
    glCreateBuffers(1, &vbo); //create {1} vbo; put the id in {2}
    glNamedBufferData(vbo, sizeof(positions), &positions, GL_DYNAMIC_DRAW); // bind a {2} bytes memory area to the currently bound array({1}) buffer; write {2} bytes reading from {3}; usage hints are passed in {4}

    //the vertex array contains vertices with the following attribs:
    glBindVertexArray(vao);
    glBindVertexBuffer(0, vbo, 0, sizeof(vec2)); //bind {2} as the {1}th vbo to the active vao, starting from {3} within the buffer. the stride between vertices is {4}
    glEnableVertexAttribArray(0); //enable the attrib 0({1}) (can be done after call to glVertexAttribPointer I think)
    glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, 0); //the attrib 0({1}) is 2({2}) floats({3}); they do not({4}) need normalization; {5} is the offset of this attrib from the start of the vertex. This data references the bound vbo and is stored in the bound vao
    glVertexAttribBinding(0, 0); // bind the attrib {1} to the {2}th vbo of the active vao

    uint ibo; //index buffer object
    glCreateBuffers(1, &ibo); //create {1} ibo; put the id in {2}
    glNamedBufferData(ibo, sizeof(indices), &indices, GL_STATIC_DRAW); // bind a {2} bytes memory area to the currently bound ibo ({1}); write {2} bytes reading from {3}; usage hints are passed in {4}

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

            const f32 mod = .8f;
            auto &p = positions[i];

            p.x = mod * std::sin(theta);
            p.y = mod * std::cos(theta);
        }

        glNamedBufferData(vbo, sizeof(positions), &positions, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0); //vbo needn't be bound for the draw, unlike vba


        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); //"select" {2} and treat it as an ibo ({1})
        glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_INT, nullptr); //draw triangles({1}), read {2} elements of type {3} either from {4} or, if {4} is null from the bound GL_ELEMENT_ARRAY_BUFFER
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        window.swap_buffers();
        window.poll_events();
    }

} catch (std::exception& e) {
    scluk::out("Caught exception!\n %", e.what());
}


