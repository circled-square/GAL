add_subdirectory(window/)

#scene
add_library(application__scene STATIC scene.cpp)
target_link_libraries(application__scene PRIVATE application__window)
target_include_directories(application__scene PUBLIC ../../include/)

#application
add_library(application STATIC application.cpp)
target_link_libraries(application PRIVATE third_party__imgui internal__graphics__init)
target_link_libraries(application PUBLIC application__scene application__window third_party__scluk)
target_include_directories(application PUBLIC ../../include/)
