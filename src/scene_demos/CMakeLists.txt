add_library(demos__scene_with_previous_scene STATIC scene_with_previous_scene/scene_with_previous_scene.cpp)
target_link_libraries(demos__scene_with_previous_scene PRIVATE application__scene third_party__imgui)

set(DEMOS_LIBS demos__scene_with_previous_scene third_party__imgui stb_image_wrapper gl_types gl_renderer gl_texture)

add_library(demos__menu STATIC menu_demo/menu_demo.cpp)
target_link_libraries(demos__menu PRIVATE ${DEMOS_LIBS})

add_library(demos__mul_obj STATIC mul_obj_draw_demo/mul_obj_draw_demo.cpp)
target_link_libraries(demos__mul_obj PRIVATE ${DEMOS_LIBS})

add_library(demos__clear_color STATIC clear_color_demo/clear_color_demo.cpp)
target_link_libraries(demos__clear_color PRIVATE ${DEMOS_LIBS})

add_library(demos__texture STATIC texture_demo/texture_demo.cpp)
target_link_libraries(demos__texture PRIVATE ${DEMOS_LIBS})

add_library(demos__3d STATIC 3d_demo/3d_demo.cpp)
target_link_libraries(demos__3d PRIVATE ${DEMOS_LIBS})

add_library(demos__gltf STATIC gltf_demo/gltf_demo.cpp)
target_link_libraries(demos__gltf PRIVATE ${DEMOS_LIBS} third_party__tinygltf)

unset(DEMOS_LIBS)

add_library(scene_demos INTERFACE)
target_link_libraries(scene_demos INTERFACE demos__menu demos__mul_obj demos__clear_color demos__texture demos__3d demos__gltf)
