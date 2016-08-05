cmake_minimum_required(VERSION 3.2)
project(boiler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/)

# Shader files
set(SHADER_FILES data/shaders/shader.vert data/shaders/shader.frag)

# boiler sources
add_subdirectory(src/boiler)

# other sources
add_subdirectory(src/terrain)

# run executable target
add_custom_target(run
  COMMAND lldb -o run build/src/terrain/terrain
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
