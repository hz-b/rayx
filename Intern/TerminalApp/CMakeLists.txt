cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(TerminalApp VERSION 0.5.0.0)
add_executable(${PROJECT_NAME} main.cpp TerminalApp.cpp TerminalApp.h CommandParser.cpp)

# GIT Versioning
message(STATUS "Resolving GIT Version...")
set(_build_version "unknown")

find_package(Git)

if(GIT_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE _build_version
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  message(STATUS "GIT hash: ${_build_version}")
else()
  message(STATUS "GIT not found")
endif()

string(TIMESTAMP _time_stamp)

configure_file(TerminalAppConfig.h.in TerminalAppConfig.h)

# CLI11
# Command Parser
target_link_libraries(${PROJECT_NAME} 
  PRIVATE CLI11::CLI11
  PRIVATE RayCore
  PRIVATE ${PYTHON_LIBRARIES}
)

target_include_directories(${PROJECT_NAME} 
    PRIVATE ${RayCore_SOURCE_DIR}/src
    PRIVATE ${PROJECT_BINARY_DIR}
)