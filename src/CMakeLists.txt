cmake_minimum_required(VERSION 3.26)
project(GLstuff)
find_package(PkgConfig)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG_BUILD")

pkg_check_modules(GLFW3 REQUIRED glfw3)
pkg_check_modules(GLEW REQUIRED glew)

##### print stuff #####
function(print_all_variables)
    get_cmake_property(_variableNames VARIABLES)
    list (SORT _variableNames)
    foreach (_variableName ${_variableNames})
        if (_variableName MATCHES "glfw" OR _variableName MATCHES "GLFW")
            message(STATUS "${_variableName}=${${_variableName}}")
        endif()
    endforeach()
    # Get all propreties that cmake supports
    if(NOT CMAKE_PROPERTY_LIST)
        execute_process(COMMAND cmake --help-property-list OUTPUT_VARIABLE CMAKE_PROPERTY_LIST)

        # Convert command output into a CMake list
        string(REGEX REPLACE ";" "\\\\;" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
        string(REGEX REPLACE "\n" ";" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
    endif()
endfunction()
#print_all_variables()
##### end print stuff

#set(IMGUI_FILES "third_party/imgui/imgui.cpp" "third_party/imgui/imgui_demo.cpp" "third_party/imgui/imgui_draw.cpp" "third_party/imgui/imgui_tables.cpp" "third_party/imgui/imgui_widgets.cpp")
file(GLOB IMGUI_FILES "third_party/imgui/*.cpp")
set(IMGUI_BACKENDS_FILES "third_party/imgui/backends/imgui_impl_glfw.cpp" "third_party/imgui/backends/imgui_impl_opengl3.cpp")
add_library(imgui STATIC ${IMGUI_FILES} ${IMGUI_BACKENDS_FILES})

file(GLOB_RECURSE SCLUK_FILES "third_party/SCLUK/include/scluk/definition/*.cpp")
add_library(scluk STATIC ${SCLUK_FILES})
add_library(stb_image STATIC third_party/stb_image.cpp)

add_library(stb_image_wrapper STATIC stb/image.cpp)
add_library(glfw_wrapper STATIC glfw/window.cpp)
add_library(gl_error_handling STATIC gl/error_handling.cpp)
add_library(gl_shader STATIC gl/shader.cpp)
add_library(gl_buffer STATIC gl/buffer.cpp)
add_library(gl_renderer STATIC gl/renderer.cpp)
add_library(gl_vertex_array STATIC gl/vertex_array.cpp)
add_library(gl_texture STATIC gl/texture.cpp)

add_executable(main main.cpp)

target_include_directories(main PRIVATE third_party)
target_include_directories(imgui PUBLIC third_party/imgui) # imgui internally expects this
target_include_directories(main PRIVATE third_party/SCLUK/include)
target_include_directories(stb_image_wrapper PRIVATE third_party)
target_include_directories(stb_image PRIVATE third_party)

target_link_libraries(main glfw_wrapper)
target_link_libraries(glfw_wrapper ${GLFW3_LIBRARIES})

target_link_libraries(main ${GLEW_LIBRARIES})

target_link_libraries(main imgui)

target_link_libraries(main scluk)

target_link_libraries(main stb_image_wrapper)
target_link_libraries(stb_image_wrapper stb_image)

target_link_libraries(main gl_error_handling)
target_link_libraries(main gl_renderer)
target_link_libraries(gl_renderer gl_shader)
target_link_libraries(gl_renderer gl_vertex_array)
target_link_libraries(gl_vertex_array gl_buffer)
target_link_libraries(main gl_texture)