#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
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

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

using namespace scluk;
using namespace glm;

int main() try {
    glfw::window window({800, 600}, "Hello OpenGL");

    if(glewInit() != GLEW_OK)
        throw scluk::runtime_error("GLEW failed to initialize!");

    gl::initialize_error_handling();

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();
    ImGuiIO& imgui_io = ImGui::GetIO();
    imgui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    glEnable(GL_BLEND);

    out("%", glGetString(GL_VERSION));

    vec3 model_position_1 = vec3(0,0,0);
    vec3 model_position_2 = vec3(1,1,0);
    mat4 model_mat_1 = glm::translate(mat4(1.0f), model_position_1); //translation, rotation, scale of the model?
    mat4 model_mat_2 = glm::translate(mat4(1.0f), model_position_2); //translation, rotation, scale of the model?
    vec3 cam_position = vec3(1,0,0);
    mat4 view_mat = glm::translate(mat4(1.0f), -cam_position); // translation, rotation, scale of the camera
    mat4 proj_mat = glm::ortho(-2.f, 2.f, -1.5f, 1.5f, -1.f, 1.f); // aspect ratio dependant coordinates -> normalized device coordinates
    mat4 mvp_mat_1 = proj_mat * view_mat * model_mat_1;
    mat4 mvp_mat_2 = proj_mat * view_mat * model_mat_2;

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
    tex.bind(0);

    shader.set_uniform<int>("u_texture_slot", 0);


    bool keep_going = true;
    window.set_key_cb([&](glfw::window_t, int k, int, int, int) {
        keep_going = keep_going && k != GLFW_KEY_ESCAPE;
    });
    auto start = std::chrono::high_resolution_clock::now();

    bool show_demo_window = false;
    vec3 clear_color(0);
    float slider_value = 0.0f;
    int counter = 0;
    while (!window.should_close() && keep_going) {

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if(show_demo_window)
            ImGui::ShowDemoWindow();

        {
            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state

            ImGui::SliderFloat("float", &slider_value, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", &clear_color[0]); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.1f FPS", imgui_io.Framerate);
            ImGui::End();
        }


        const f32 seconds = std::chrono::duration<f32, std::ratio<1>>(std::chrono::high_resolution_clock::now()- start).count();

        //rotate the vertex_data (this of course could just be a uniform but i wanted to experiment with vao.specify_attribs<...>()
        for (auto& v: vertex_data)
            v.angle = pi<f32>() * sin(seconds / 2.0f);

        vao.vbo.update(vertex_data);

        glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
        renderer.clear();

        shader.set_uniform("u_color", vec4(.8, .4, .4, 1.0));
        shader.set_uniform("u_mvp", mvp_mat_1);
        renderer.draw(vao, shader, 0, 2);

        shader.set_uniform("u_color", vec4(.4, .4, .8, .5));
        shader.set_uniform("u_mvp", mvp_mat_2);
        renderer.draw(vao, shader, 0, 2);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.swap_buffers();
        window.poll_events();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

} catch (std::exception& e) {
    scluk::out("Caught exception!\n %", e.what());
}


