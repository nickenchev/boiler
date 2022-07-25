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
#add_subdirectory(${GLEW_LIB_DIR}/build/cmake)
set(GLEW_HEADERS_DIR ${GLEW_LIB_DIR}/include)

# build zlib
set(ZLIB_DIR ${LIBS_ROOT}/zlib-1.2.11)
add_subdirectory(${ZLIB_DIR} ${CMAKE_CURRENT_BINARY_DIR}/zlib_build)

# build freetype2
add_subdirectory(${LIBS_ROOT}/freetype-2.12.1)

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
elseif(WIN32)
  set(VULKAN_INCLUDE_DIR ${VULKAN_SDK}/Include)
  set(VULKAN_LIBS ${VULKAN_SDK}/Lib/vulkan-1.lib)
else()
  # Linux
  set(VULKAN_INCLUDE_DIR ${VULKAN_SDK}/x86_64/include)
  set(VULKAN_LIBS ${VULKAN_SDK}/x86_64/lib/libvulkan.so ${VULKAN_SDK}/x86_64/lib/libVkLayer_khronos_validation.so)
endif()

message(${CMAKE_CURRENT_BINARY_DIR}/zlib_build)

include_directories(zlib ${CMAKE_CURRENT_BINARY_DIR}/zlib_build)
set(boiler_lib_includes
  ${OPENGL_INCLUDE_DIR}
  ${VULKAN_INCLUDE_DIR}
  ${GLEW_HEADERS_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}/zlib_build
  ${ZLIB_DIR}
  ${JSONCPP_INCLUDE_DIR}
  glm
  freetype
  fmt
  boiler-gltf)

target_include_directories(boiler_deps PUBLIC ${boiler_lib_includes})

target_link_libraries(boiler_deps
  zlibstatic
  freetype
  glm
  #glew_s
  jsoncpp_lib
  fmt
  ${VULKAN_LIBS}
  boiler-gltf)
