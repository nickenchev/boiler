cmake_minimum_required(VERSION 3.2)
project(engine)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin/)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(ZLIB REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GSL REQUIRED)

include_directories(
  ${SDL2_INCLUDE_DIR}
  ${SDL2_IMAGE_INCLUDE_DIR}
  ${ZLIB_INCLUDE_DIR}
  ${GSL_INCLUDE_DIR}
  ${OPENGL_INCLUDE_DIR}
  ${GLEW_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/include/tinyxml2
  ${PROJECT_SOURCE_DIR}/src/boiler
  ${PROJECT_SOURCE_DIR}/src/boiler/core
  ${PROJECT_SOURCE_DIR}/src/boiler/containers
  ${PROJECT_SOURCE_DIR}/src/boiler/input
  ${PROJECT_SOURCE_DIR}/src/boiler/video
  ${PROJECT_SOURCE_DIR}/src/boiler/camera
  ${PROJECT_SOURCE_DIR}/src/boiler/util)

# Shader files
set(SHADER_FILES data/shaders/shader.vert data/shaders/shader.frag)

# Engine related sources
add_library(boiler
  src/boiler/core/engine.cpp
  src/boiler/core/entity.cpp
  src/boiler/core/part.cpp
  src/boiler/core/component.cpp
  src/boiler/core/tmxloader.cpp
  src/boiler/core/spriteloader.cpp
  src/boiler/core/spritesheet.cpp
  src/boiler/core/spritesheetframe.cpp
  src/boiler/core/base64.cpp
  src/boiler/core/quadtree.cpp
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
  src/boiler/camera/camera.cpp
  src/boiler/camera/platformercamera.cpp
  src/boiler/containers/gridposition.cpp
  src/boiler/util/tinyxml2.cpp
  src/boiler/util/jsoncpp.cpp)

# Game related sources
add_library(game
  src/main.cpp
  src/terrain/terrainpart.cpp)

add_executable(${PROJECT_NAME} ${SHADER_FILES})
target_link_libraries(${PROJECT_NAME} game boiler ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${ZLIB_LIBRARY} ${OPENGL_LIBRARY} ${GLEW_LIBRARY})
