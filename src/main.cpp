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

    uint buffer;
    //init the buffer in VRAM
    glGenBuffers(1, &buffer); //generate {1} buffer; put the id in {2}
    glBindBuffer(GL_ARRAY_BUFFER, buffer); //"select" {2} and treat it as an array ({1})
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), &positions, GL_DYNAMIC_DRAW); // bind a {2} bytes memory area to the currently bound array({1}) buffer; write {2} bytes reading from {3}; usage hints are passed in {4}

    //the vertex array contains vertices with the following attribs:
    glEnableVertexAttribArray(0); //enable the attrib 0({1}) (can be done after call to glVertexAttribPointer I think)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (const void *)0); //at index 0({1}), 2({2}) floats({3}); they do not({4}) need normalization; {5} is the size of each vertex, {6} is the offset of this attrib from the start of the vertex

    uint ibo; //index buffer object
    //init the ibo in VRAM
    glGenBuffers(1, &ibo); //generate {1} ibo; put the id in {2}
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); //"select" {2} and treat it as an array ({1})
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW); // bind a {2} bytes memory area to the currently bound ibo ({1}); write {2} bytes reading from {3}; usage hints are passed in {4}

    gl::shader_program shader(read_file("shader/vert.glsl"), read_file("shader/frag.glsl"));
    shader.use();

    bool keep_going = true;
    window.set_key_cb([&](glfw::window_t, int k, int, int, int) { 
        keep_going = keep_going && k != GLFW_KEY_ESCAPE;
    });
    

    auto start = std::chrono::high_resolution_clock::now();

    while (!window.should_close() && keep_going) {
        const f32 seconds = std::chrono::duration<f32, std::ratio<1>>(std::chrono::high_resolution_clock::now()- start).count();


        for (int i: index(positions)) {
            const f32 theta = seconds + pi / 2.0f * i;

            const f32 mod = .8f;
            auto &p = positions[i];

            p.x = mod * std::sin(theta);
            p.y = mod * std::cos(theta);
        }

        glBindBuffer(GL_ARRAY_BUFFER, buffer);                                                   
        glBufferData(GL_ARRAY_BUFFER, sizeof(positions), &positions, GL_DYNAMIC_DRAW);


        glClear(GL_COLOR_BUFFER_BIT);
        //glDrawArrays(GL_TRIANGLES, 0, positions.size() * 3); // render triangle primitives ({1}) from the bound array ibo, from index {2} read {3} elements
        glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_INT, nullptr); //{1} like above, read {2} elements of type {3} either from {4} or if it is null from the bound GL_ELEMENT_ARRAY_BUFFER

        window.swap_buffers();
        window.poll_events();
    }

} catch (std::exception& e) {
    scluk::out("Caught exception!\n %", e.what());
}


