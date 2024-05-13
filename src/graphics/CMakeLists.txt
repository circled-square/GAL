add_library(graphics__shader STATIC shader.cpp)
target_link_libraries(graphics__shader PUBLIC internal__graphics__types)
target_include_directories(graphics__shader PUBLIC ../../include/)

add_library(graphics__buffer STATIC buffer/buffer.cpp buffer/vertex_buffer.cpp buffer/index_buffer.cpp) # vertex/index_buffer are thin wrappers of buffer
target_link_libraries(graphics__buffer PUBLIC internal__graphics__types)
target_include_directories(graphics__buffer PUBLIC ../../include/)

add_library(graphics__vertex_array STATIC vertex_array.cpp)
target_link_libraries(graphics__vertex_array PUBLIC internal__graphics__types graphics__buffer)
target_include_directories(graphics__vertex_array PUBLIC ../../include/)

add_library(graphics__renderer STATIC renderer/renderer.cpp renderer/retro_renderer.cpp)
target_link_libraries(graphics__renderer PRIVATE graphics__vertex_array graphics__shader)
target_link_libraries(graphics__renderer PUBLIC internal__graphics__types)
target_include_directories(graphics__renderer PUBLIC ../../include/)

add_library(graphics__texture STATIC texture.cpp)
target_link_libraries(graphics__texture PUBLIC internal__graphics__types)
target_include_directories(graphics__texture PUBLIC ../../include/)

add_library(graphics__image STATIC image.cpp)
target_link_libraries(graphics__image PRIVATE third_party__stb)
target_include_directories(graphics__image PUBLIC ../../include/)


add_library(graphics INTERFACE)
target_link_libraries(graphics INTERFACE graphics__shader graphics__buffer graphics__vertex_array graphics__renderer graphics__texture graphics__image)
