cmake_minimum_required(VERSION 3.25)

project(GL4RenderingEngine VERSION 0.1.0 LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG_BUILD")

add_subdirectory(include/)
add_subdirectory(demo/)
