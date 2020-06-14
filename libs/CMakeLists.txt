add_library(boiler_deps null.cpp)

# Disable shared linking, link all statically
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Disable all shared libs")
option(BUILD_SHARED_LIBS OFF)

# build glm
set(GLM_TEST_ENABLE OFF)
add_subdirectory(${LIBS_ROOT}/glm-0.9.9.8)

# build glew
set(GLEW_STATIC ON CACHE BOOL "Use GLEW static lib")
set(GLEW_LIB_DIR ${LIBS_ROOT}/glew-2.1.0)
add_subdirectory(${GLEW_LIB_DIR}/build/cmake)
set(GLEW_HEADERS_DIR ${GLEW_LIB_DIR}/include)

# build zlib
set(ZLIB_DIR ${LIBS_ROOT}/zlib-1.2.11)
add_subdirectory(${ZLIB_DIR})
set(ZLIB_INCLUDE_DIR ${ZLIB_DIR})
set(PNG_CONFIG_HEADER ${CMAKE_CURRENT_BINARY_DIR}/zlib-1.2.11)

# build SDL2
set(SDL_SHARED OFF CACHE BOOL "Disable SDL2 shared lib")
add_subdirectory(${LIBS_ROOT}/SDL2-2.0.12)

# build libpng
set(PNG_SHARED OFF CACHE BOOL "Disable PNG shared lib")
set(PNG_STATIC ON CACHE BOOL "Enable PNG static lib")
set(PNG_BUILD_ZLIB ON CACHE BOOL "Use Boiler's ZLIB")
set(PNG_DIR ${LIBS_ROOT}/libpng16)
add_subdirectory(${PNG_DIR})
set(PNG_INCLUDE_DIR ${PNG_DIR})
set(PNG_CONFIG_HEADER ${CMAKE_CURRENT_BINARY_DIR}/libpng16)

# build freetype2
add_subdirectory(${LIBS_ROOT}/freetype-2.10.1)

# build jsoncpp
set(JSONCPP_WITH_TESTS CACHE OFF "Disable JSONCPP unit tests")
set(JSONCPP_WITH_POST_BUILD_UNITTEST CACHE OFF "Disable JSONCPP post-build runner")
add_subdirectory(${LIBS_ROOT}/jsoncpp)

# build fmt
add_subdirectory(${LIBS_ROOT}/fmt-6.0.0)

# boiler-gltf
add_subdirectory(${LIBS_ROOT}/boiler-gltf)

# Vulkan
if (APPLE)
  set(VULKAN_INCLUDE_DIR ${VULKAN_SDK}/include)
  set(VULKAN_LIBS ${VULKAN_SDK}/lib/libvulkan.dylib)
else()
  # Vulkan
  set(VULKAN_INCLUDE_DIR ${VULKAN_SDK}/x86_64/include)
  set(VULKAN_LIBS ${VULKAN_SDK}/x86_64/lib/libvulkan.so ${VULKAN_SDK}/x86_64/lib/libVkLayer_khronos_validation.so)
endif()

set(boiler_lib_includes
  ${OPENGL_INCLUDE_DIR}
  ${VULKAN_INCLUDE_DIR}
  ${GLEW_HEADERS_DIR}
  ${PNG_INCLUDE_DIR}
  ${ZLIB_INCLUDE_DIR}
  ${JSONCPP_INCLUDE_DIR}
  png
  ${PNG_CONFIG_HEADER}
  glm
  SDL2
  freetype
  fmt
  boiler-gltf)

target_include_directories(boiler_deps PUBLIC ${boiler_lib_includes})

target_link_libraries(boiler_deps
  SDL2-static
  png_static
  zlibstatic
  freetype
  glm
  glew_s
  jsoncpp_lib
  fmt
  ${VULKAN_LIBS}
  boiler-gltf)
