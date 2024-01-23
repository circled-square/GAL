cmake_minimum_required(VERSION 3.25)

project(GL4RenderingEngine VERSION 0.1.0 LANGUAGES CXX C)

# git submodule update
if($ENV{DO_SUBMODULE_UPDATE})
    find_package(Git QUIET)
    if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
        # Update submodules as needed
        option(GIT_SUBMODULE "Check submodules during build" ON)
        if(GIT_SUBMODULE)
            message(STATUS "Submodule update")
            execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive --remote
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE GIT_SUBMOD_RESULT)
            if(NOT GIT_SUBMOD_RESULT EQUAL "0")
                message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
            endif()
        endif()
    else()
        message(STATUS "could not find git to update submodules; continuing")
    endif()
endif()


add_subdirectory(src)
