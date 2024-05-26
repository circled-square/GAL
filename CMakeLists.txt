cmake_minimum_required(VERSION 3.25)

project(GAL VERSION 0.1.0 LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src/)
add_subdirectory(private/)

add_library(GAL INTERFACE)
target_link_libraries(GAL INTERFACE application graphics third_party__imgui)
target_include_directories(GAL INTERFACE include/)
