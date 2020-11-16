cmake_minimum_required(VERSION 3.2)
project(boiler CXX)

include(ExternalProject)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 20)
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
set(INCLUDE_BASE ${CMAKE_CURRENT_LIST_DIR}/include)
set(BOILER_HEADERS
  ${INCLUDE_BASE}/boiler.h
  ${INCLUDE_BASE}/core/components/guicomponent.h
  ${INCLUDE_BASE}/core/components/lightingcomponent.h
  ${INCLUDE_BASE}/core/components/materialcomponent.h
  ${INCLUDE_BASE}/core/components/rendercomponent.h
  ${INCLUDE_BASE}/core/components/parentcomponent.h
  ${INCLUDE_BASE}/core/components/positioncomponent.h
  ${INCLUDE_BASE}/core/components/rendercomponent.h
  ${INCLUDE_BASE}/core/components/spritecomponent.h
  ${INCLUDE_BASE}/core/components/textcomponent.h
  ${INCLUDE_BASE}/camera/camera.h
  ${INCLUDE_BASE}/camera/pancamera.h
  ${INCLUDE_BASE}/camera/platformercamera.h
  ${INCLUDE_BASE}/containers/array2d.h
  ${INCLUDE_BASE}/containers/gridposition.h
  #${INCLUDE_BASE}/core/glyphloader.h
  ${INCLUDE_BASE}/video/opengl.h
  ${INCLUDE_BASE}/video/renderer.h
  ${INCLUDE_BASE}/video/lightsource.h
  ${INCLUDE_BASE}/video/texture.h
  ${INCLUDE_BASE}/assets/gltfimporter.h
)

# Engine related sources
set(SRC_BASE ${CMAKE_CURRENT_LIST_DIR}/src)
set(BOILER_SOURCES
  ${SRC_BASE}/core/engine.cpp
  ${SRC_BASE}/core/entity.cpp
  ${SRC_BASE}/core/entityworld.cpp
  ${SRC_BASE}/core/tmxloader.cpp
  ${SRC_BASE}/core/spritesheet.cpp
  ${SRC_BASE}/core/spriteframe.cpp
  ${SRC_BASE}/core/base64.cpp
  #${SRC_BASE}/core/glyphloader.cpp
  ${SRC_BASE}/core/quadtree.cpp
  ${SRC_BASE}/core/rect.cpp
  #${SRC_BASE}/core/raycaster.cpp
  ${SRC_BASE}/core/ray.cpp
  ${SRC_BASE}/core/bmfont.cpp
  ${SRC_BASE}/core/spriteanimation.cpp
  ${SRC_BASE}/core/line.cpp
  ${SRC_BASE}/core/fileutils.cpp
  ${SRC_BASE}/video/renderer.cpp
  ${SRC_BASE}/video/primitive.cpp
  # Vulkan Rendering
  ${SRC_BASE}/video/vulkanrenderer.cpp

  # Platform Support
  ${SRC_BASE}/platform/sdlwindow.cpp

  # Video and Rendering
  #${SRC_BASE}/video/openglrenderer.cpp
  #${SRC_BASE}/video/opengltexture.cpp
  #${SRC_BASE}/video/openglmodel.cpp
  #${SRC_BASE}/video/glslshaderprogram.cpp
  ${SRC_BASE}/video/spvshaderprogram.cpp
  ${SRC_BASE}/video/imaging/imageloader.cpp
  ${SRC_BASE}/video/systems/rendersystem.cpp
  ${SRC_BASE}/video/systems/glyphsystem.cpp
  ${SRC_BASE}/video/systems/guisystem.cpp
  ${SRC_BASE}/video/systems/lightingsystem.cpp
  ${SRC_BASE}/video/glyphmap.cpp
  ${SRC_BASE}/camera/camera.cpp
  # src/camera/platformercamera.cpp
  ${SRC_BASE}/camera/pancamera.cpp
  ${SRC_BASE}/containers/gridposition.cpp
  ${SRC_BASE}/util/filemanager.cpp
  ${SRC_BASE}/util/tinyxml2.cpp
  ${SRC_BASE}/assets/gltfimporter.cpp)

add_library(boiler STATIC ${BOILER_SOURCES} ${BOILER_HEADERS})

target_include_directories(boiler
  INTERFACE
  ${INCLUDE_BASE}
  PRIVATE
  ${INCLUDE_BASE}
  ${INCLUDE_BASE}/tinyxml2
  boiler_deps)

target_link_libraries(boiler boiler_deps)

#target_compile_options(boiler PRIVATE -Wall -Wextra)
