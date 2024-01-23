add_subdirectory(glfw/)
add_library(third_party__glfw ALIAS glfw)

add_subdirectory(SCLUK/)
add_library(third_party__scluk ALIAS scluk)

add_library(third_party__glm INTERFACE)
target_include_directories(third_party__glm INTERFACE glm/)

add_library(third_party__glad STATIC glad/src/glad.c)
target_include_directories(third_party__glad PUBLIC glad/include)

#imgui
file(GLOB IMGUI_FILES "imgui/*.cpp")
set(IMGUI_BACKENDS_FILES "imgui/backends/imgui_impl_glfw.cpp" "imgui/backends/imgui_impl_opengl3.cpp")
add_library(third_party__imgui STATIC ${IMGUI_FILES} ${IMGUI_BACKENDS_FILES})
target_link_libraries(third_party__imgui PRIVATE third_party__glfw) #for imgui_impl_glfw
target_include_directories(third_party__imgui PUBLIC imgui/)

#stb
add_library(third_party__stb STATIC stb.cpp)
target_include_directories(third_party__stb PUBLIC ./)

#tinygltf
add_library(third_party__tinygltf STATIC tinygltf.cpp)
target_link_libraries(third_party__tinygltf PRIVATE third_party__stb)
target_include_directories(third_party__tinygltf PUBLIC ./)
