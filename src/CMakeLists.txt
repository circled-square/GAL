cmake_minimum_required(VERSION 3.26)
project(GLstuff LANGUAGES CXX C)
find_package(PkgConfig)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG_BUILD")

pkg_check_modules(GLFW3 REQUIRED glfw3)
pkg_check_modules(GLEW REQUIRED glew)

#imgui
file(GLOB IMGUI_FILES "third_party/imgui/*.cpp")
set(IMGUI_BACKENDS_FILES "third_party/imgui/backends/imgui_impl_glfw.cpp" "third_party/imgui/backends/imgui_impl_opengl3.cpp")
add_library(imgui STATIC ${IMGUI_FILES} ${IMGUI_BACKENDS_FILES})
target_include_directories(imgui PUBLIC third_party/imgui) # imgui internally expects this; it's public because if any other stuff includes imgui it needs to include this or else it will get errors for all the '#include "imgui.h"' imgui contains


#scluk
file(GLOB_RECURSE SCLUK_FILES "third_party/SCLUK/include/scluk/definition/*.cpp")
add_library(scluk STATIC ${SCLUK_FILES})
target_include_directories(scluk PUBLIC third_party/SCLUK/include)


#stb
add_library(stb_image STATIC third_party/stb_image.cpp)
target_include_directories(stb_image PUBLIC third_party)

add_library(stb_image_wrapper STATIC stb/image.cpp)
target_link_libraries(stb_image_wrapper stb_image)


#glfw
add_library(glfw_wrapper STATIC glfw/window.cpp)
target_link_libraries(glfw_wrapper ${GLFW3_LIBRARIES})


#gl
add_library(gl_initialize_opengl STATIC gl/initialize_opengl.cpp)
target_link_libraries(gl_initialize_opengl ${GLEW_LIBRARIES} scluk)

add_library(gl_shader STATIC gl/shader.cpp)
target_link_libraries(gl_shader ${GLEW_LIBRARIES} scluk)

add_library(gl_buffer STATIC gl/buffer.cpp gl/vertex_buffer.cpp gl/index_buffer.cpp) # vertex_buffer and index_buffer are just thin wrappers over buffer
target_link_libraries(gl_buffer ${GLEW_LIBRARIES})

add_library(gl_vertex_array STATIC gl/vertex_array.cpp)
target_link_libraries(gl_vertex_array gl_buffer)

add_library(gl_renderer STATIC gl/renderer.cpp)
target_link_libraries(gl_renderer gl_vertex_array gl_shader)

add_library(gl_texture STATIC gl/texture.cpp)
target_link_libraries(gl_texture ${GLEW_LIBRARIES})

add_library(scene STATIC scene_loader/scene.cpp)
target_link_libraries(scene gl_texture gl_renderer stb_image_wrapper imgui) # these libs are not actually used by scene itself

add_library(scene_loader STATIC scene_loader/scene_loader.cpp)
target_link_libraries(scene_loader imgui)

add_library(scene_with_previous_scene STATIC scene_demos/scene_with_previous_scene.cpp)
target_link_libraries(scene_with_previous_scene scene imgui)

add_library(mul_obj_draw_demo STATIC scene_demos/mul_obj_draw_demo.cpp)
target_link_libraries(mul_obj_draw_demo scene_with_previous_scene imgui)

add_library(menu_demo STATIC scene_demos/menu_demo.cpp)
target_link_libraries(menu_demo scene scene_with_previous_scene imgui)

add_executable(main main.cpp)
target_link_libraries(main scluk glfw_wrapper imgui gl_initialize_opengl scene_loader menu_demo mul_obj_draw_demo)
