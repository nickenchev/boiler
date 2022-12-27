cmake_minimum_required(VERSION 3.2)
project(boiler CXX)

include(ExternalProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/)

find_package(SDL2 REQUIRED)
find_package(Freetype REQUIRED)
find_package(Vulkan REQUIRED)
find_package(ZLIB REQUIRED)

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
  ${INCLUDE_BASE}/core/common.h
  ${INCLUDE_BASE}/core/glyphloader.h
  ${INCLUDE_BASE}/core/logger.h
  ${INCLUDE_BASE}/core/components/guicomponent.h
  ${INCLUDE_BASE}/core/components/lightingcomponent.h
  ${INCLUDE_BASE}/core/components/materialcomponent.h
  ${INCLUDE_BASE}/core/components/rendercomponent.h
  ${INCLUDE_BASE}/core/components/parentcomponent.h
  ${INCLUDE_BASE}/core/components/transformcomponent.h
  ${INCLUDE_BASE}/core/components/rendercomponent.h
  ${INCLUDE_BASE}/core/components/spritecomponent.h
  ${INCLUDE_BASE}/core/components/textcomponent.h
  ${INCLUDE_BASE}/core/matrixcache.h
  ${INCLUDE_BASE}/camera/camera.h
  ${INCLUDE_BASE}/camera/pancamera.h
  ${INCLUDE_BASE}/camera/platformercamera.h
  ${INCLUDE_BASE}/containers/array2d.h
  ${INCLUDE_BASE}/containers/gridposition.h
  ${INCLUDE_BASE}/display/systems/textsystem.h
  ${INCLUDE_BASE}/display/systems/debugrendersystem.h
  ${INCLUDE_BASE}/display/opengl.h
  ${INCLUDE_BASE}/display/renderer.h
  ${INCLUDE_BASE}/display/lightsource.h
  ${INCLUDE_BASE}/display/texture.h
  ${INCLUDE_BASE}/display/imaging/imagedata.h
  ${INCLUDE_BASE}/display/imaging/skyboximages.h
  ${INCLUDE_BASE}/assets/gltfimporter.h
  ${INCLUDE_BASE}/display/vulkan/graphicspipeline.h
  ${INCLUDE_BASE}/display/vulkan/graphicspipelinebuilder.h
  ${INCLUDE_BASE}/display/vulkan/shaderstagemodules.h
  ${INCLUDE_BASE}/display/skyboxloader.h
  ${INCLUDE_BASE}/input/inputsystem.h
  ${INCLUDE_BASE}/input/inputcomponent.h
  ${INCLUDE_BASE}/assets/maps/maploader.h
  ${INCLUDE_BASE}/assets/maps/gamemap.h
  # Animation related
  ${INCLUDE_BASE}/animation/animation.h
  ${INCLUDE_BASE}/animation/animationsampler.h
  ${INCLUDE_BASE}/animation/animator.h
  ${INCLUDE_BASE}/animation/channel.h
  ${INCLUDE_BASE}/animation/clip.h
  ${INCLUDE_BASE}/animation/common.h
  ${INCLUDE_BASE}/animation/components/animationcomponent.h
  ${INCLUDE_BASE}/animation/systems/animationsystem.h
  ${INCLUDE_BASE}/json/jsonloader.h
  ${INCLUDE_BASE}/physics/collidercomponent.h
  ${INCLUDE_BASE}/physics/collisioncomponent.h
  ${INCLUDE_BASE}/physics/movementsystem.h
  ${INCLUDE_BASE}/physics/physicscomponent.h
  ${INCLUDE_BASE}/physics/physicssystem.h
  ${INCLUDE_BASE}/physics/aabb.h
  ${INCLUDE_BASE}/physics/sphere.h
  ${INCLUDE_BASE}/camera/camerasystem.h
)

# Engine related sources
set(SRC_BASE ${CMAKE_CURRENT_LIST_DIR}/src)
set(BOILER_SOURCES
  ${SRC_BASE}/core/engine.cpp
  ${SRC_BASE}/core/entityworld.cpp
  ${SRC_BASE}/core/entitycomponentsystem.cpp
  ${SRC_BASE}/core/tmxloader.cpp
  ${SRC_BASE}/core/spritesheet.cpp
  ${SRC_BASE}/core/spriteframe.cpp
  ${SRC_BASE}/core/base64.cpp
  ${SRC_BASE}/core/glyphloader.cpp
  ${SRC_BASE}/core/logger.cpp
  #${SRC_BASE}/core/quadtree.cpp
  ${SRC_BASE}/core/rect.cpp
  #${SRC_BASE}/core/raycaster.cpp
  #${SRC_BASE}/core/ray.cpp
  ${SRC_BASE}/core/bmfont.cpp
  ${SRC_BASE}/core/spriteanimation.cpp
  ${SRC_BASE}/core/line.cpp
  ${SRC_BASE}/core/fileutils.cpp
  ${SRC_BASE}/display/renderer.cpp
  ${SRC_BASE}/display/primitive.cpp
  # Vulkan Rendering
  ${SRC_BASE}/display/vulkanrenderer.cpp
  ${SRC_BASE}/display/vulkan/graphicspipeline.cpp

  # Platform Support
  ${SRC_BASE}/platform/sdlwindow.cpp

  # display and Rendering
  #${SRC_BASE}/display/openglrenderer.cpp
  #${SRC_BASE}/display/opengltexture.cpp
  #${SRC_BASE}/display/openglmodel.cpp
  #${SRC_BASE}/display/glslshaderprogram.cpp
  ${SRC_BASE}/display/spvshaderprogram.cpp
  ${SRC_BASE}/display/skyboxloader.cpp
  ${SRC_BASE}/display/imaging/stb.cpp
  ${SRC_BASE}/display/imaging/imageloader.cpp
  ${SRC_BASE}/display/systems/rendersystem.cpp
  ${SRC_BASE}/display/systems/textsystem.cpp
  ${SRC_BASE}/display/systems/lightingsystem.cpp
  ${SRC_BASE}/animation/systems/animationsystem.cpp
  ${SRC_BASE}/display/systems/debugrendersystem.cpp
  ${SRC_BASE}/display/glyphmap.cpp
  ${SRC_BASE}/camera/camera.cpp
  # src/camera/platformercamera.cpp
  ${SRC_BASE}/camera/pancamera.cpp
  ${SRC_BASE}/containers/gridposition.cpp
  ${SRC_BASE}/util/filemanager.cpp
  ${SRC_BASE}/util/tinyxml2.cpp
  ${SRC_BASE}/assets/gltfimporter.cpp
  ${SRC_BASE}/assets/maploader.cpp
  ${SRC_BASE}/physics/movementsystem.cpp
  ${SRC_BASE}/physics/physicssystem.cpp
  ${SRC_BASE}/physics/aabb.cpp
  ${SRC_BASE}/physics/sphere.cpp
  ${SRC_BASE}/camera/camerasystem.cpp
  ${SRC_BASE}/input/inputsystem.cpp
  # ImGui
  ${SRC_BASE}/display/systems/guisystem.cpp
  ${SRC_BASE}/ext/imgui/imgui.cpp
  ${SRC_BASE}/ext/imgui/imgui_draw.cpp
  ${SRC_BASE}/ext/imgui/imgui_tables.cpp
  ${SRC_BASE}/ext/imgui/imgui_widgets.cpp
  ${SRC_BASE}/ext/imgui/imgui_demo.cpp
  ${SRC_BASE}/ext/imgui/imgui_impl_vulkan.cpp)

add_library(boiler STATIC ${BOILER_SOURCES} ${BOILER_HEADERS})

target_include_directories(boiler
  INTERFACE
  ${SRC_BASE}/ext
  ${INCLUDE_BASE}
  PRIVATE
  ${INCLUDE_BASE}
  ${INCLUDE_BASE}/tinyxml2
  Freetype::Freetype
  ${ZLIB_INCLUDE_DIRS}
  boiler_deps)

#target_compile_options(boiler PRIVATE -Werror -Wall -Wextra)

target_link_libraries(boiler
  boiler_deps
  SDL2::SDL2 SDL2::SDL2main
  Freetype::Freetype
  Vulkan::Vulkan
  ${ZLIB_LIBRARIES}
)
