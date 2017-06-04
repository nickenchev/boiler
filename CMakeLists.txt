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
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(ZLIB REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

include_directories(${PROJECT_SOURCE_DIR/include})

# Engine related headers
set(BOILER_HEADERS
  ${PROJECT_SOURCE_DIR}/include/video/renderer.h)

# Engine related sources
set(BOILER_SOURCES
  src/boiler/core/boiler.cpp
  src/boiler/core/entity.cpp
  src/boiler/core/entityworld.cpp
  src/boiler/core/part.cpp
  src/boiler/core/component.cpp
  src/boiler/core/tmxloader.cpp
  src/boiler/core/spriteloader.cpp
  src/boiler/core/spritesheet.cpp
  src/boiler/core/spritesheetframe.cpp
  src/boiler/core/base64.cpp
  # src/boiler/core/quadtree.cpp
  src/boiler/core/rect.cpp
  src/boiler/core/raycaster.cpp
  src/boiler/core/ray.cpp
  src/boiler/core/bmfont.cpp
  src/boiler/core/spriteanimation.cpp
  src/boiler/core/line.cpp
  src/boiler/core/fileutils.cpp
  src/boiler/video/renderer.cpp
  src/boiler/video/model.cpp
  src/boiler/video/fontloader.cpp
  src/boiler/video/vertexdata.cpp
  src/boiler/video/openglrenderer.cpp
  src/boiler/video/opengltexture.cpp
  src/boiler/video/openglmodel.cpp
  src/boiler/video/shaderprogram.cpp
  src/boiler/video/imageloader.cpp
  src/boiler/video/systems/rendersystem.cpp
  src/boiler/camera/camera.cpp
  # src/boiler/camera/platformercamera.cpp
  src/boiler/camera/pancamera.cpp
  src/boiler/containers/gridposition.cpp
  src/boiler/util/filemanager.cpp
  src/boiler/util/tinyxml2.cpp
  src/boiler/util/jsoncpp.cpp)

add_library(boiler ${BOILER_SOURCES})

target_include_directories(boiler PUBLIC
  ${PROJECT_SOURCE_DIR}/src/boiler
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/include/tinyxml2
  ${SDL2_INCLUDE_DIR}
  ${SDL2_IMAGE_INCLUDE_DIR}
  ${ZLIB_INCLUDE_DIR}
  ${OPENGL_INCLUDE_DIR}
  ${GLEW_INCLUDE_DIR})

target_link_libraries(boiler ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${ZLIB_LIBRARY} ${OPENGL_LIBRARY} ${GLEW_LIBRARY})

add_subdirectory(src/blank)
