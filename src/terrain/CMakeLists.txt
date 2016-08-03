add_executable(terrain main.cpp terrainpart.cpp)

target_include_directories(terrain PUBLIC ${BOILER_INCLUDE_DIRS})
target_link_libraries(terrain boiler)
