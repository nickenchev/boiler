cmake_minimum_required(VERSION 3.2)
project(boiler)

include(ExternalProject)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++14")
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/)
set(OpenGL_GL_PREFERENCE "GLVND")

# Shader files
set(SHADER_FILES data/shaders/basic.vert data/shaders/basic.frag)

# Build libraries
if (NOT DEFINED ${LIBS_ROOT})
  set(LIBS_ROOT ${CMAKE_CURRENT_LIST_DIR}/libs)
endif()

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Disable all shared libs" FORCE)
# build glm
add_subdirectory(${LIBS_ROOT}/glm-0.9.9.5)
# build glew
set(GLEW_STATIC ON CACHE BOOL "Use GLEW static lib")
add_subdirectory(${LIBS_ROOT}/glew-2.1.0/build/cmake)
# build zlib
add_subdirectory(${LIBS_ROOT}/zlib-1.2.11)
# build SDL2
set(SDL_SHARED OFF CACHE BOOL "Disable SDL2 shared lib" FORCE)
add_subdirectory(${LIBS_ROOT}/SDL2-2.0.9)
# build libpng
add_subdirectory(${LIBS_ROOT}/libpng16)
# build freetype2
add_subdirectory(${LIBS_ROOT}/freetype2-VER-2-10-0)

# boiler sources
find_package(OpenGL REQUIRED)

# Engine related headers
set(BOILER_HEADERS
  ${CMAKE_CURRENT_LIST_DIR}/include/boiler.h
  ${CMAKE_CURRENT_LIST_DIR}/include/camera/camera.h
  ${CMAKE_CURRENT_LIST_DIR}/include/camera/pancamera.h
  ${CMAKE_CURRENT_LIST_DIR}/include/camera/platformercamera.h
  ${CMAKE_CURRENT_LIST_DIR}/include/containers/array2d.h
  ${CMAKE_CURRENT_LIST_DIR}/include/containers/gridposition.h
  ${CMAKE_CURRENT_LIST_DIR}/include/core/glyphloader.h
  ${CMAKE_CURRENT_LIST_DIR}/include/video/renderer.h)

# Engine related sources
set(BOILER_SOURCES
  ${CMAKE_CURRENT_LIST_DIR}/src/core/engine.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/core/entityworld.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/core/tmxloader.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/core/spriteloader.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/core/spritesheet.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/core/spritesheetframe.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/core/base64.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/core/glyphloader.cpp
  # src/core/quadtree.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/core/rect.cpp
  #${CMAKE_CURRENT_LIST_DIR}/src/core/raycaster.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/core/ray.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/core/bmfont.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/core/spriteanimation.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/core/line.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/core/fileutils.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/video/renderer.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/video/model.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/video/fontloader.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/video/vertexdata.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/video/openglrenderer.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/video/opengltexture.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/video/openglmodel.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/video/shaderprogram.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/video/imageloader.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/video/systems/rendersystem.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/video/systems/glyphsystem.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/video/systems/guisystem.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/video/glyphmap.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/camera/camera.cpp
  # src/camera/platformercamera.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/camera/pancamera.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/containers/gridposition.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/util/filemanager.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/util/tinyxml2.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/util/jsoncpp.cpp)

add_library(boiler ${BOILER_SOURCES} ${BOILER_HEADERS})

target_include_directories(boiler PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/include/boiler
  ${CMAKE_CURRENT_LIST_DIR}/include
  ${CMAKE_CURRENT_LIST_DIR}/include/tinyxml2
  ${OPENGL_INCLUDE_DIR}
  glm
  zlib
  SDL2
  png
  freetype)

target_link_libraries(boiler SDL2 png zlibstatic freetype glm glew ${OPENGL_LIBRARY})
