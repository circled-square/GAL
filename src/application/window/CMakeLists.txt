add_library(application__window STATIC window.cpp)
target_link_libraries(application__window PUBLIC third_party__glad third_party__glm)
target_link_libraries(application__window PRIVATE third_party__glfw)
target_include_directories(application__window PUBLIC ../../../include/)
