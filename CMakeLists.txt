cmake_minimum_required(VERSION 3.2)
project(boiler CXX)

include(ExternalProject)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/)

# Build libraries
if (NOT DEFINED ${LIBS_ROOT})
  set(LIBS_ROOT ${CMAKE_CURRENT_LIST_DIR}/libs)
endif()

add_subdirectory(${LIBS_ROOT})

# Shader files
set(SHADER_FILES data/shaders/basic.vert data/shaders/basic.frag)

# boiler sources
# Engine related headers
set(BOILER_HEADERS
  ${CMAKE_CURRENT_LIST_DIR}/include/boiler.h
  ${CMAKE_CURRENT_LIST_DIR}/include/camera/camera.h
  ${CMAKE_CURRENT_LIST_DIR}/include/camera/pancamera.h
  ${CMAKE_CURRENT_LIST_DIR}/include/camera/platformercamera.h
  ${CMAKE_CURRENT_LIST_DIR}/include/containers/array2d.h
  ${CMAKE_CURRENT_LIST_DIR}/include/containers/gridposition.h
  ${CMAKE_CURRENT_LIST_DIR}/include/core/glyphloader.h
  ${CMAKE_CURRENT_LIST_DIR}/include/video/opengl.h
  ${CMAKE_CURRENT_LIST_DIR}/include/video/renderer.h)

# Engine related sources
set(BOILER_SOURCES
  ${CMAKE_CURRENT_LIST_DIR}/src/core/engine.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/core/entityworld.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/core/tmxloader.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/core/spritesheet.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/core/spritesheetframe.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/core/base64.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/core/glyphloader.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/core/quadtree.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/core/rect.cpp
  #${CMAKE_CURRENT_LIST_DIR}/src/core/raycaster.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/core/ray.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/core/bmfont.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/core/spriteanimation.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/core/line.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/core/fileutils.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/video/renderer.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/video/model.cpp
  # Vulkan Rendering
  ${CMAKE_CURRENT_LIST_DIR}/src/video/vulkanrenderer.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/video/vulkanmodel.cpp

  # Platform Support
  ${CMAKE_CURRENT_LIST_DIR}/src/platform/sdlwindow.cpp

  # Video and Rendering
  ${CMAKE_CURRENT_LIST_DIR}/src/video/openglrenderer.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/video/opengltexture.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/video/openglmodel.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/video/glslshaderprogram.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/video/spvshaderprogram.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/video/imaging/imageloader.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/video/systems/rendersystem.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/video/systems/glyphsystem.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/video/systems/guisystem.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/video/systems/lightingsystem.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/video/glyphmap.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/camera/camera.cpp
  # src/camera/platformercamera.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/camera/pancamera.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/containers/gridposition.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/util/filemanager.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/util/tinyxml2.cpp)

add_library(boiler STATIC ${BOILER_SOURCES} ${BOILER_HEADERS})

target_include_directories(boiler PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/include
  ${CMAKE_CURRENT_LIST_DIR}/include/tinyxml2
  PUBLIC boiler_deps)

target_link_libraries(boiler boiler_deps)

#target_compile_options(boiler PRIVATE -Wall -Wextra)
