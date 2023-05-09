#include <GL/glew.h>

#include <scluk/language_extension.hpp>
#include <stdexcept>

#include "glfw/window.hpp"
#include "gl/initialize_opengl.hpp"
#include "scene_loader/scene_loader.hpp"
#include "scene_demos/mul_obj_draw_demo.hpp"
#include "scene_demos/menu_demo.hpp"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

using namespace scluk;
using namespace glm;

int main() try {
    glfw::window window({800, 600}, "Hello there");

    gl::initialize_opengl();

    out("%", glGetString(GL_VERSION));

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    ImGui::StyleColorsDark();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    std::vector<scene_demos::named_scene> scene_vector;
    scene_vector.push_back({ std::unique_ptr<scene_loader::scene>(new scene_demos::mul_obj_draw_demo), "multiple object draw demo" });

    scene_demos::menu_demo menu_demo(std::move(scene_vector));

    scene_loader::scene_loader active_scene_demo_manager(window, &menu_demo);

    active_scene_demo_manager.run();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

} catch (std::exception& e) {
    out("Caught exception!\n %", e.what());
}


