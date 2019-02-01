cmake_minimum_required(VERSION 3.2)
project(boiler)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/)
set(OpenGL_GL_PREFERENCE "GLVND")

# Shader files
set(SHADER_FILES data/shaders/shader.vert data/shaders/shader.frag)

# boiler sources
find_package(glm REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(ZLIB REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Freetype REQUIRED)

# Engine related headers
set(BOILER_HEADERS
  ${CMAKE_CURRENT_LIST_DIR}/include/camera/camera.h
  ${CMAKE_CURRENT_LIST_DIR}/include/camera/pancamera.h
  ${CMAKE_CURRENT_LIST_DIR}/include/camera/platformercamera.h
  ${CMAKE_CURRENT_LIST_DIR}/include/containers/array2d.h
  ${CMAKE_CURRENT_LIST_DIR}/include/containers/gridposition.h
  ${CMAKE_CURRENT_LIST_DIR}/include/video/renderer.h)

# Engine related sources
set(BOILER_SOURCES
  ${CMAKE_CURRENT_LIST_DIR}/src/core/boiler.cpp
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
  ${GLM_INCLUDE_DIR}
  ${SDL2_INCLUDE_DIR}
  ${SDL2_IMAGE_INCLUDE_DIR}
  ${ZLIB_INCLUDE_DIR}
  ${OPENGL_INCLUDE_DIR}
  ${GLEW_INCLUDE_DIR}
  ${FREETYPE_INCLUDE_DIRS})

target_link_libraries(boiler ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${ZLIB_LIBRARY} ${OPENGL_LIBRARY} ${GLEW_LIBRARY} ${FREETYPE_LIBRARIES})
