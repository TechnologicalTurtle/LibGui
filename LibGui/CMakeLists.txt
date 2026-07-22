# setup this file
cmake_minimum_required(VERSION 3.23)
include(FetchContent)

project(LibGui)

# project build settings
add_library(LibGui)

set_target_properties(LibGui PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON.
)

# fetch glfw and freetype from net
FetchContent_Declare(  glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL  "")
set(GLFW_INSTALL OFF CACHE BOOL  "")
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(  freetype
        GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
        GIT_TAG        VER-2-14-3
)
FetchContent_MakeAvailable(freetype)

# source settings
target_link_libraries(LibGui
  PRIVATE
    glfw
    freetype
)
target_include_directories(LibGui PRIVATE
  ${GLFW_SOURCE_DIR}/include
  libraries/include
)

target_sources(LibGui
  PRIVATE
    libraries/include/glad/glad.h
    libraries/include/KHR/khrplatform.h
    libraries/include/stb/stb_image.h
    libraries/include/stb/stb_image_write.h
    libraries/lib/glad.c
    libraries/lib/stb_image.cpp
    libraries/lib/stb_image_write.cpp
        src/LibGui.cpp
  PUBLIC
    FILE_SET headers
    TYPE HEADERS
    FILES
        include/LibGui.hpp
)
