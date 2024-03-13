cmake_minimum_required(VERSION 3.25)

project(GAL VERSION 0.1.0 LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG_BUILD")

add_subdirectory(src/)
add_subdirectory(private/)

add_library(GAL INTERFACE)
target_link_libraries(GAL INTERFACE application graphics third_party__imgui third_party__tinygltf)
target_include_directories(GAL INTERFACE include/)
