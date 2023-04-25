cmake_minimum_required(VERSION 3.26)
project(GLstuff)
find_package(PkgConfig)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG_BUILD")

pkg_check_modules(GLFW3 REQUIRED glfw3)
pkg_check_modules(GLEW REQUIRED glew)
add_library(imgui STATIC third_party/imgui/imgui.cpp)
set(SCLUK_DIR third_party/SCLUK/include/scluk/definition)
add_library(scluk STATIC ${SCLUK_DIR}/exception.cpp ${SCLUK_DIR}/logging.cpp ${SCLUK_DIR}/math.cpp ${SCLUK_DIR}/range.cpp ${SCLUK_DIR}/string.cpp)
add_library(stb_image STATIC third_party/stb_image/stb_image.cpp)

add_library(stb_image_wrapper STATIC stb/image.cpp)
add_library(glfw_wrapper STATIC glfw/window.cpp)
add_library(gl_error_handling STATIC gl/error_handling.cpp)
add_library(gl_shader STATIC gl/shader.cpp)
add_library(gl_buffer STATIC gl/buffer.cpp)
add_library(gl_renderer STATIC gl/renderer.cpp)
add_library(gl_vertex_array STATIC gl/vertex_array.cpp)
add_library(gl_texture STATIC gl/texture.cpp)

add_executable(main main.cpp)

target_include_directories(main PRIVATE third_party)
target_include_directories(main PRIVATE third_party/SCLUK/include)

target_link_libraries(main glfw_wrapper)
target_link_libraries(glfw_wrapper ${GLFW3_LIBRARIES})

target_link_libraries(main ${GLEW_LIBRARIES})

target_link_libraries(main imgui)

target_link_libraries(main scluk)

target_link_libraries(main stb_image_wrapper)
target_link_libraries(stb_image_wrapper stb_image)

target_link_libraries(main gl_error_handling)
target_link_libraries(main gl_renderer)
target_link_libraries(gl_renderer gl_shader)
target_link_libraries(gl_renderer gl_vertex_array)
target_link_libraries(gl_vertex_array gl_buffer)
