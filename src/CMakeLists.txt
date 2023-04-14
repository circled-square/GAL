cmake_minimum_required(VERSION 3.26)
project(GLstuff)
find_package(PkgConfig)
pkg_check_modules(GLFW3 REQUIRED glfw3)
pkg_check_modules(GLEW REQUIRED glew)
set(CMAKE_CXX_STANDARD 23)

add_library(glfw_wrapper STATIC glfw/window.cpp)
add_library(shader_manager STATIC gl/shader.cpp)

add_executable(main main.cpp vulkan.cpp)

target_include_directories(main PRIVATE ~/.local/include) #scluk is included from here
target_link_libraries(main ${GLFW3_LIBRARIES})
target_link_libraries(main ${GLEW_LIBRARIES})
target_link_libraries(main glfw_wrapper)
target_link_libraries(main shader_manager)
target_link_libraries(main ${CMAKE_CURRENT_SOURCE_DIR}/scluk.a)
