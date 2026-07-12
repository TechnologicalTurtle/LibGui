cmake_minimum_required(VERSION 4.1)

project(LibGui)

add_library(LibGui)

set_target_properties(LibGui PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON.
)

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
      src/LibGui.h
)
