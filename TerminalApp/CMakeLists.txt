cmake_minimum_required(VERSION 3.18.3)
project(TerminalApp VERSION 0.1.0)

file(GLOB CPP_FILES src/*.cpp)

add_definitions(${RayCore_DEFINITIONS})

include_directories(${RayCore_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${CPP_FILES})

target_link_libraries(${PROJECT_NAME} RayCore)