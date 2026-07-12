cmake_minimum_required(VERSION 3.23)

project(Client)

add_executable(Client)

target_link_libraries(Client
      LibGui
)

target_sources(Client
   PRIVATE
      Main.cpp
)