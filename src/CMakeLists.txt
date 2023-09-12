cmake_minimum_required(VERSION 3.25)

project(GL4RenderingEngine LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG_BUILD")


add_subdirectory(third_party/glfw)
include_directories(third_party/glm)

add_library(glad STATIC third_party/glad/src/glad.c)
target_include_directories(glad PUBLIC third_party/glad/include)

#imgui
file(GLOB IMGUI_FILES "third_party/imgui/*.cpp")
set(IMGUI_BACKENDS_FILES "third_party/imgui/backends/imgui_impl_glfw.cpp" "third_party/imgui/backends/imgui_impl_opengl3.cpp")
add_library(imgui STATIC ${IMGUI_FILES} ${IMGUI_BACKENDS_FILES})
target_link_libraries(imgui glfw) #for imgui_impl_glfw
target_include_directories(imgui PUBLIC third_party/imgui)


#scluk
file(GLOB_RECURSE SCLUK_FILES "third_party/SCLUK/include/scluk/definition/*.cpp")
add_library(scluk STATIC ${SCLUK_FILES})
target_include_directories(scluk PUBLIC third_party/SCLUK/include)

#stb
add_library(stb STATIC third_party/stb.cpp)
target_include_directories(stb PUBLIC third_party)

#tinygltf
add_library(tinygltf STATIC third_party/tinygltf.cpp)
target_link_libraries(tinygltf stb)
target_include_directories(tinygltf PUBLIC third_party)

add_library(stb_image_wrapper STATIC stb/image.cpp)
target_link_libraries(stb_image_wrapper stb)


#glfw
add_library(glfw_wrapper STATIC glfw/window.cpp)
target_link_libraries(glfw_wrapper glfw scluk)


#gl
add_library(gl_initialize_opengl STATIC gl/initialize_opengl.cpp)
target_link_libraries(gl_initialize_opengl glad glfw scluk)

add_library(gl_shader STATIC gl/shader.cpp)
target_link_libraries(gl_shader glad scluk)

add_library(gl_buffer STATIC gl/buffer.cpp gl/vertex_buffer.cpp gl/index_buffer.cpp) # vertex_buffer and index_buffer are just thin wrappers over buffer
target_link_libraries(gl_buffer glad scluk)

add_library(gl_vertex_array STATIC gl/vertex_array.cpp)
target_link_libraries(gl_vertex_array gl_buffer)

add_library(gl_renderer STATIC gl/renderer.cpp)
target_link_libraries(gl_renderer gl_vertex_array gl_shader)

add_library(gl_texture STATIC gl/texture.cpp)
target_link_libraries(gl_texture glad scluk)


#application
add_library(scene STATIC application/scene.cpp)
target_link_libraries(scene gl_texture gl_renderer stb_image_wrapper imgui) # these libs are not actually used by scene itself

add_library(application STATIC application/application.cpp)
target_link_libraries(application glfw_wrapper imgui gl_initialize_opengl scene)


#scene demos
add_library(scene_with_previous_scene STATIC scene_demos/scene_with_previous_scene.cpp)
target_link_libraries(scene_with_previous_scene scene imgui)


add_library(menu_demo STATIC scene_demos/menu_demo.cpp)
target_link_libraries(menu_demo scene scene_with_previous_scene imgui)

add_library(mul_obj_draw_demo STATIC scene_demos/mul_obj_draw_demo.cpp)
target_link_libraries(mul_obj_draw_demo scene_with_previous_scene imgui)

add_library(clear_color_demo STATIC scene_demos/clear_color_demo.cpp)
target_link_libraries(clear_color_demo scene_with_previous_scene imgui)

add_library(texture_demo STATIC scene_demos/texture_demo.cpp)
target_link_libraries(texture_demo scene_with_previous_scene imgui)

add_library(3d_demo STATIC scene_demos/3d_demo.cpp)
target_link_libraries(3d_demo scene_with_previous_scene imgui)

add_library(gltf_demo STATIC scene_demos/gltf_demo.cpp)
target_link_libraries(gltf_demo scene_with_previous_scene imgui tinygltf)

set(SCENE_DEMOS menu_demo mul_obj_draw_demo clear_color_demo texture_demo 3d_demo gltf_demo)


add_executable(main main.cpp)
target_link_libraries(main application ${SCENE_DEMOS})
