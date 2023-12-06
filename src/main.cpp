#include <stdexcept>
#include <iostream>

#include "application/application.hpp"

#include "scene_demos/menu_demo/menu_demo.hpp"
#include "scene_demos/mul_obj_draw_demo/mul_obj_draw_demo.hpp"
#include "scene_demos/clear_color_demo/clear_color_demo.hpp"
#include "scene_demos/texture_demo/texture_demo.hpp"
#include "scene_demos/3d_demo/3d_demo.hpp"
#include "scene_demos/gltf_demo/gltf_demo.hpp"

using namespace glm;

int main() try {
    scene_demos::menu_demo menu_demo;

    menu_demo.add_scene<scene_demos::mul_obj_draw_demo>("multiple object draw demo");
    menu_demo.add_scene<scene_demos::clear_color_demo>("clear color demo");
    menu_demo.add_scene<scene_demos::texture_demo>("texture demo");
    menu_demo.add_scene<scene_demos::three_dimensional_demo>("3d demo");
    menu_demo.add_scene<scene_demos::gltf_demo>("gltf demo");

    application::application application({800, 600}, "demo", &menu_demo);

    menu_demo.activate("gltf demo"); // only call this after opengl is initialised

    std::cout << glGetString(GL_VERSION) << std::endl;

    application.run();
} catch (std::exception& e) {
    std::cerr << "Caught exception!\n" << e.what() << std::endl;
}
