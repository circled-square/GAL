set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG_BUILD")

add_subdirectory(third_party/)
add_subdirectory(gl/)

#stb_image_wrapper
add_library(stb_image_wrapper STATIC stb/image.cpp)
target_link_libraries(stb_image_wrapper PRIVATE third_party__stb)


add_subdirectory(scene_demos/)
add_subdirectory(application/)


add_executable(main main.cpp)
target_link_libraries(main PRIVATE application scene_demos)
