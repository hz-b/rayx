cmake_minimum_required(VERSION 3.18.3)
project(TerminalApp VERSION 0.1.0)


include_directories(${RayCore_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} src/TerminalApp.cpp)

target_link_libraries(${PROJECT_NAME} RayCore)