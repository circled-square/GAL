#include <GL/glew.h>

#include <stdexcept>
#include <iostream>

#include "glfw/window.hpp"
#include "gl/initialize_opengl.hpp"
#include "scene_loader/scene_loader.hpp"
#include "scene_demos/mul_obj_draw_demo.hpp"
#include "scene_demos/clear_color_demo.hpp"
#include "scene_demos/menu_demo.hpp"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

using namespace glm;
using namespace std;

int main() try {
    glfw::window window({800, 600}, "Hello there");

    gl::initialize_opengl();

    cout << glGetString(GL_VERSION) << endl;

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    ImGui::StyleColorsDark();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;


    scene_demos::menu_demo menu_demo;
    menu_demo.add_scene<scene_demos::mul_obj_draw_demo>("multiple object draw demo");
    menu_demo.add_scene<scene_demos::clear_color_demo>("clear color demo");

    scene_loader::scene_loader active_scene_demo_manager(window, &menu_demo);

    active_scene_demo_manager.run();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

} catch (::exception& e) {
    cout << "Caught exception!\n" << e.what() << endl;
}


