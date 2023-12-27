add_library(gl_types INTERFACE)
target_link_libraries(gl_types INTERFACE third_party__glad third_party__scluk third_party__glm)

add_library(gl_initialize_opengl STATIC initialize_opengl.cpp)
target_link_libraries(gl_initialize_opengl PRIVATE third_party__glad third_party__glfw third_party__scluk)

add_library(gl_shader STATIC shader.cpp)
target_link_libraries(gl_shader PUBLIC gl_types)

add_library(gl_buffer STATIC buffer.cpp vertex_buffer.cpp index_buffer.cpp) # vertex/index_buffer are thin wrappers of buffer
target_link_libraries(gl_buffer PUBLIC gl_types)

add_library(gl_vertex_array STATIC vertex_array.cpp)
target_link_libraries(gl_vertex_array PUBLIC gl_types gl_buffer)

add_library(gl_renderer STATIC renderer.cpp)
target_link_libraries(gl_renderer PRIVATE gl_vertex_array gl_shader gl_types)

add_library(gl_texture STATIC texture.cpp)
target_link_libraries(gl_texture PUBLIC gl_types)

