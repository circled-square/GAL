#include <iostream>

#include "../include/application/application.hpp"

#include "menu_demo/menu_demo.hpp"
#include "mul_obj_draw_demo/mul_obj_draw_demo.hpp"
#include "clear_color_demo/clear_color_demo.hpp"
#include "texture_demo/texture_demo.hpp"
#include "3d_demo/3d_demo.hpp"
#include "gltf_demo/gltf_demo.hpp"

#include <scluk/log.hpp>

using namespace glm;

int main() {
    scene_demos::menu_demo menu_demo;
    menu_demo.add_scene<scene_demos::mul_obj_draw_demo>("multiple object draw demo");
    menu_demo.add_scene<scene_demos::clear_color_demo>("clear color demo");
    menu_demo.add_scene<scene_demos::texture_demo>("texture demo");
    menu_demo.add_scene<scene_demos::three_dimensional_demo>("3d demo");
    menu_demo.add_scene<scene_demos::gltf_demo>("gltf demo");

    gal::application::application application({1920, 1080}, "demo", &menu_demo);
    scluk::stdout_log("%", glGetString(GL_VERSION));

    menu_demo.activate("gltf demo"); // only call this after opengl is initialised

    application.run();
}
