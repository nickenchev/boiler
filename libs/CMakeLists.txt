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

# build jsoncpp
set(JSONCPP_WITH_TESTS CACHE OFF "Disable JSONCPP unit tests")
set(JSONCPP_WITH_POST_BUILD_UNITTEST CACHE OFF "Disable JSONCPP post-build runner")
add_subdirectory(${LIBS_ROOT}/jsoncpp)

# build fmt
add_subdirectory(${LIBS_ROOT}/fmt-9.1.0)

# boiler-gltf
add_subdirectory(${LIBS_ROOT}/boiler-gltf)

message(${CMAKE_CURRENT_BINARY_DIR}/zlib_build)

include_directories(zlib ${CMAKE_CURRENT_BINARY_DIR}/zlib_build)
set(boiler_lib_includes
  ${OPENGL_INCLUDE_DIR}
  ${GLEW_HEADERS_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}/zlib_build
  ${ZLIB_DIR}
  ${JSONCPP_INCLUDE_DIR}
  glm
  fmt
  boiler-gltf)

target_include_directories(boiler_deps PUBLIC ${boiler_lib_includes})

target_link_libraries(boiler_deps
  zlibstatic
  glm
  #glew_s
  jsoncpp_lib
  fmt
  boiler-gltf)
