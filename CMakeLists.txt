cmake_minimum_required(VERSION 3.2)
project(mygame)

#set(CMAKE_CXX_COMPILER /usr/local/Cellar/llvm/3.6.2/bin/clang++)
#set(CMAKE_CXX_COMPILER /usr/bin/g++-5)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_MODULE_PATH cmake/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin/)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(ZLIB REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

include_directories(
  ${SDL2_INCLUDE_DIR}
  ${SDL2_IMAGE_INCLUDE_DIR}
  ${ZLIB_INCLUDE_DIR}
  ${OPENGL_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/include/tinyxml2
  ${GLEW_INCLUDE_DIR})

# Engine related sources
add_library(boiler
  src/engine.cpp
  src/renderer.cpp
  src/openglrenderer.cpp
  src/opengltexture.cpp
  src/tmxloader.cpp
  src/jsoncpp.cpp
  src/spriteloader.cpp
  src/spritesheet.cpp
  src/spritesheetframe.cpp
  src/component.cpp
  src/tinyxml2.cpp
  src/base64.cpp
  src/shaderprogram.cpp
  src/entity.cpp
  src/quadtree.cpp
  src/rect.cpp
  src/raycaster.cpp
  src/bmfont.cpp
  src/part.cpp
  src/spriteanimation.cpp
  src/line.cpp
  src/camera.cpp
  src/platformercamera.cpp)

# Game related sources
set(SHADER_FILES data/shaders/shader.vert data/shaders/shader.frag)
set(SRC_FILES
  src/main.cpp
  src/gamepart.cpp)

add_executable(${PROJECT_NAME} ${SRC_FILES} ${SHADER_FILES})
target_link_libraries(${PROJECT_NAME} boiler ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${ZLIB_LIBRARY} ${OPENGL_LIBRARY} ${GLEW_LIBRARY})
