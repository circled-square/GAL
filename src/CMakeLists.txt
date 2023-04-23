cmake_minimum_required(VERSION 3.26)
project(GLstuff)
find_package(PkgConfig)
pkg_check_modules(GLFW3 REQUIRED glfw3)
pkg_check_modules(GLEW REQUIRED glew)
set(CMAKE_CXX_STANDARD 23)

set(
        CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG_BUILD"
)

add_library(glfw_wrapper STATIC glfw/window.cpp)
add_library(gl_error_handling STATIC gl/error_handling.cpp)
add_library(gl_shader STATIC gl/shader.cpp)
add_library(gl_buffer STATIC gl/buffer.cpp)
add_library(gl_vertex_array STATIC gl/vertex_array.cpp)

add_executable(main main.cpp)

target_include_directories(main PRIVATE ~/.local/include) #scluk is included from here
target_link_libraries(main ${GLFW3_LIBRARIES})
target_link_libraries(main ${GLEW_LIBRARIES})
target_link_libraries(main glfw_wrapper)
target_link_libraries(main gl_error_handling)
target_link_libraries(main gl_shader)
target_link_libraries(main gl_buffer)
target_link_libraries(main gl_vertex_array)
target_link_libraries(gl_vertex_array gl_buffer)
target_link_libraries(main ${CMAKE_CURRENT_SOURCE_DIR}/scluk.a)
