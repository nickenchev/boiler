find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(ZLIB REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# Engine related sources
add_library(boiler
  core/engine.cpp
  core/entity.cpp
  core/part.cpp
  core/component.cpp
  core/tmxloader.cpp
  core/spriteloader.cpp
  core/spritesheet.cpp
  core/spritesheetframe.cpp
  core/base64.cpp
  core/quadtree.cpp
  core/rect.cpp
  core/raycaster.cpp
  core/ray.cpp
  core/bmfont.cpp
  core/spriteanimation.cpp
  core/line.cpp
  core/fileutils.cpp
  video/renderer.cpp
  video/model.cpp
  video/fontloader.cpp
  video/vertexdata.cpp
  video/openglrenderer.cpp
  video/opengltexture.cpp
  video/openglmodel.cpp
  video/shaderprogram.cpp
  video/imageloader.cpp
  camera/camera.cpp
  camera/platformercamera.cpp
  camera/pancamera.cpp
  containers/gridposition.cpp
  util/filemanager.cpp
  util/tinyxml2.cpp
  util/jsoncpp.cpp)

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
