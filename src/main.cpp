#include <GL/glew.h>
#include <scluk/language_extension.hpp>
#include <scluk/exception.hpp>
#include <scluk/array.hpp>
#include <glm/glm.hpp>
#include "glfw/window.hpp"

int create_shader(std::string vertexSh, std::string fragmentSh);
uint compile_shader(uint type, std::string source);

int main() try {
    using namespace scluk::language_extension;
    using scluk::array;
    using namespace glm;

    glfw::window window({640, 480}, "Hello World");

    if(glewInit() != GLEW_OK)
        throw scluk::runtime_error("GLEW failed to initialize!");

    out("%", glGetString(GL_VERSION));

    const array<vec2,3> positions = {
        vec2
        {-0.5f, -0.5f},
        {+0.0f, +0.5f},
        {+0.5f, -0.5f}
    };
    uint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), &positions, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (const void *)0);

    const char vs[] =
    "#version 330 core\n"
    "layout(location = 0) in vec4 position;"
    "void main() {"
        "gl_Position = position;"
    "}", 
    fs[]=
    "#version 330 core\n"
    "layout(location = 0) out vec4 color;"
    "void main() {"
        "color = vec4(1.0, 0.0, 0.0, 1.0);"
    "}";

    uint shader = create_shader(vs, fs);
    glUseProgram(shader);

    bool keep_going = true;
    window.set_key_cb([&](glfw::window_t, int k, int, int, int) { 
        keep_going = keep_going && k != GLFW_KEY_ESCAPE;
    });

    while (!window.should_close() && keep_going) {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0,3);

        window.swap_buffers();
        window.poll_events();
    }

    glDeleteProgram(shader);
} catch (std::exception& e) {
    scluk::out("Caught exception!\n %", e.what());
}


int create_shader(std::string vertexSh, std::string fragmentSh) {
    uint program = glCreateProgram();
    uint vs = compile_shader(GL_VERTEX_SHADER, std::move(vertexSh));
    uint fs = compile_shader(GL_FRAGMENT_SHADER, std::move(fragmentSh));

    glAttachShader(program, vs);
    glAttachShader(program, fs);

    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}


uint compile_shader(uint type, std::string source) {
    uint id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS,&result);
    if(!result) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);

        glDeleteShader(id);
        throw scluk::runtime_error("Failed to compile % shader\n\t%", (type == GL_VERTEX_SHADER ? "vertex" : "fragment"), message);
    }

    return id;
}
