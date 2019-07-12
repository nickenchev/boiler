add_library(boiler_deps null.cpp)

# Disable shared linking, link all statically
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Disable all shared libs")
option(BUILD_SHARED_LIBS OFF)

# build glm
set(GLM_TEST_ENABLE OFF)
add_subdirectory(${LIBS_ROOT}/glm-0.9.9.5)

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
add_subdirectory(${LIBS_ROOT}/SDL2-2.0.9)

# build libpng
set(PNG_SHARED OFF CACHE BOOL "Disable PNG shared lib")
set(PNG_STATIC ON CACHE BOOL "Enable PNG static lib")
set(PNG_BUILD_ZLIB ON CACHE BOOL "Use Boiler's ZLIB")
set(PNG_DIR ${LIBS_ROOT}/libpng16)
add_subdirectory(${PNG_DIR})
set(PNG_INCLUDE_DIR ${PNG_DIR})
set(PNG_CONFIG_HEADER ${CMAKE_CURRENT_BINARY_DIR}/libpng16)

# build freetype2
add_subdirectory(${LIBS_ROOT}/freetype2-VER-2-10-0)

# build jsoncpp
set(JSONCPP_WITH_TESTS CACHE OFF "Disable JSONCPP unit tests")
set(JSONCPP_WITH_POST_BUILD_UNITTEST CACHE OFF "Disable JSONCPP post-build runner")
add_subdirectory(${LIBS_ROOT}/jsoncpp)

# MoltenVK
set(MOLTENVK_ROOT "/Users/nenchev/Developer/frameworks/MoltenVK/")
set(MOLTENVK_INCLUDE_DIR ${MOLTENVK_ROOT}/MoltenVK/include)
set(MOLTENVK_LIBRARY ${MOLTENVK_ROOT}/MoltenVK/macOS/static/libMoltenVK.a)

find_package(OpenGL REQUIRED)

target_include_directories(boiler_deps PUBLIC
  ${OPENGL_INCLUDE_DIR}
  ${MOLTENVK_INCLUDE_DIR}
  ${GLEW_HEADERS_DIR}
  ${PNG_INCLUDE_DIR}
  ${ZLIB_INCLUDE_DIR}
  ${JSONCPP_INCLUDE_DIR}
  png
  ${PNG_CONFIG_HEADER}
  glm
  SDL2
  freetype)

target_link_libraries(boiler_deps SDL2-static png_static zlibstatic freetype glm glew_s jsoncpp_lib ${OPENGL_LIBRARY} ${MOLTENVK_LIBRARY})
