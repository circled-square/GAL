add_library(internal__graphics__init STATIC init.cpp init.hpp)
target_link_libraries(internal__graphics__init PRIVATE third_party__glfw third_party__glad third_party__scluk)
target_include_directories(internal__graphics__init PUBLIC ../../)

add_library(internal__graphics__types INTERFACE types.hpp)
target_link_libraries(internal__graphics__types INTERFACE third_party__glad third_party__glm third_party__scluk)
target_include_directories(internal__graphics__types INTERFACE ../../)
