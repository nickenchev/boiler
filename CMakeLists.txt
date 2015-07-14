cmake_minimum_required(VERSION 3.2)
project(engine)

set(CMAKE_CXX_COMPILER /usr/local/Cellar/llvm/3.6.1/bin/clang++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_MODULE_PATH cmake/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin/)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR} ${PROJECT_SOURCE_DIR}/include)

set(SRC_FILES
  src/main.cpp
  src/engine.cpp
  src/jsoncpp.cpp
  src/spriteloader.cpp
  src/engine.cpp
  src/spritesheet.cpp
  src/component.cpp)

add_executable(${PROJECT_NAME} ${SRC_FILES}) 
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY})
