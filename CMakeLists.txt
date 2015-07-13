cmake_minimum_required(VERSION 3.2)
project(engine)

set(CMAKE_CXX_COMPILER /usr/local/Cellar/llvm/3.6.1/bin/clang++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_MODULE_PATH cmake/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin/)

find_package(SDL2)
include_directories(${SDL2_INCLUDE_DIR} ${PROJECT_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME} src/main.cpp src/jsoncpp.cpp src/spriteloader.cpp)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY})
