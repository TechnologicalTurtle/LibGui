cmake_minimum_required(VERSION 3.23)

project(LibGui)

option(LIBGUI_BUILD_CLIENT "LIBGUI_BUILD_CLIENT" OFF)

#add_subdirectory(glfw)
#add_subdirectory(freetype)
add_subdirectory(LibGui)

if (LIBGUI_BUILD_CLIENT)
  message("Generating CLIENT")
  add_subdirectory(Client)
endif()