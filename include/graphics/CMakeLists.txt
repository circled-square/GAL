add_library(graphics__shader STATIC shader/shader.cpp shader/shader.hpp)
target_link_libraries(graphics__shader PUBLIC graphics_internal__types)

add_library(graphics__buffer STATIC buffer/buffer.cpp buffer/vertex_buffer.cpp buffer/index_buffer.cpp buffer/buffer.hpp buffer/vertex_buffer.hpp buffer/index_buffer.hpp) # vertex/index_buffer are thin wrappers of buffer
target_link_libraries(graphics__buffer PUBLIC graphics_internal__types)

add_library(graphics__vertex_array STATIC vertex_array/vertex_array.cpp vertex_array/vertex_array.hpp)
target_link_libraries(graphics__vertex_array PUBLIC graphics_internal__types graphics__buffer)

add_library(graphics__renderer STATIC renderer/renderer.cpp renderer/renderer.hpp)
target_link_libraries(graphics__renderer PRIVATE graphics__vertex_array graphics__shader graphics_internal__types)

add_library(graphics__texture STATIC texture/texture.cpp texture/texture.hpp)
target_link_libraries(graphics__texture PUBLIC graphics_internal__types)

add_library(graphics__image STATIC image/image.cpp image/image.hpp)
target_link_libraries(graphics__image PRIVATE third_party__stb)
