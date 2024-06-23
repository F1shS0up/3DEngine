#
# Specify minimum CMake version and project name
#
cmake_minimum_required(VERSION 3.0)
project(3DEngine)

#
# CMake setup
#
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_VERBOSE_MAKEFILE 0) # 1 should be used for debugging
set(CMAKE_SUPPRESS_REGENERATION TRUE) # Suppresses ZERO_CHECK

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++20")

    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

#
# GLFW options
#
option(GLFW_INSTALL OFF)
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(3DEngine/vendor/glfw)

#
# Set include paths
#
include_directories(3DEngine/src/
    3DEngine/gamesrc/
    3DEngine/src/ecs
    3DEngine/vendor/glad/include/
    3DEngine/vendor/glfw/include/
    3DEngine/vendor/glm/
    3DEngine/vendor/stb/
    3DEngine/vendor/box2d/include
    3DEngine/vendor/jsoncpp/include
    3DEngine/vendor/freetype/include
    3DEngine/vendor/irrKlang/include
    3DEngine/vendor/cgltf)

#
# Add files
#
file(GLOB VENDORS_SOURCES 3DEngine/vendor/glad/src/glad.c)
file(GLOB_RECURSE PROJECT_HEADERS 3DEngine/src/*.hpp
    3DEngine/src/*.h
    3DEngine/gamesrc/*.hpp
    3DEngine/gamesrc/*.h
    3DEngine/src/ecs/*.hpp
    3DEngine/src/ecs/*.h)
file(GLOB_RECURSE PROJECT_SOURCES 3DEngine/src/*.cpp
    3DEngine/src/*.cxx
    3DEngine/src/*.cc
    3DEngine/src/*.c
    3DEngine/src/ecs/*.cpp
    3DEngine/src/ecs/*.cxx
    3DEngine/src/ecs/*.cc
    3DEngine/src/ecs/*.c
    3DEngine/gamesrc/*.cpp
    3DEngine/gamesrc/*.c
    3DEngine/gamesrc/*.cc
    3DEngine/gamesrc/*.cxx)
file(GLOB PROJECT_CONFIGS CMakeLists.txt
    README.rst
    .gitignore
    .gitmodules)

find_package(Freetype REQUIRED)

#
# Organizing files
#
source_group("headers" FILES ${PROJECT_HEADERS})
source_group("sources" FILES ${PROJECT_SOURCES})
source_group("vendors" FILES ${VENDORS_SOURCES})

#
# Set executable and target link libraries
#
add_definitions(-DGLFW_INCLUDE_NONE
    -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS}
    ${PROJECT_CONFIGS}
    ${VENDORS_SOURCES})
target_link_libraries(${PROJECT_NAME}
    glfw
    ${GLFW_LIBRARIES}
    ${GLAD_LIBRARIES}
    ${PROJECT_SOURCE_DIR}/3DEngine/vendor/box2d/build/bin/Release/box2d.lib
    ${PROJECT_SOURCE_DIR}/3DEngine/vendor/jsoncpp/lib/jsoncpp.lib
    ${PROJECT_SOURCE_DIR}/3DEngine/vendor/freetype/objs/freetype.lib
    ${PROJECT_SOURCE_DIR}/3DEngine/vendor/irrKlang/lib/Winx64-visualStudio/irrKlang.lib)
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})