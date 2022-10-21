cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(TerminalApp VERSION 0.5.0.0)

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

# set(CMAKE_VERBOSE_MAKEFILE ON)
# set(CMAKE_COLOR_MAKEFILE   ON)

# Add Python files
file(COPY python DESTINATION ${CMAKE_BINARY_DIR}/bin)
add_executable(${PROJECT_NAME} main.cpp TerminalApp.cpp TerminalApp.h CommandParser.cpp)

# CLI11
# Command Parser
target_link_libraries(${PROJECT_NAME} PRIVATE CLI11::CLI11)

target_include_directories(${PROJECT_NAME} PRIVATE ${RayCore_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_BINARY_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE RayCore)

target_link_libraries(${PROJECT_NAME} PRIVATE ${PYTHON_LIBRARIES})

# find python libraries
find_package(Python3 COMPONENTS Interpreter Development NumPy REQUIRED)
find_package(PythonLibs 3.0 REQUIRED)
include_directories(${PYTHON3_INCLUDE_DIRS} ${NumPy_INCLUDE_DIRS})
include_directories(${PROJECT_NAME} PRIVATE ${RayCore_SOURCE_DIR}/../../Extern/matplotlib-cpp)

# # link python and numpy
target_link_libraries(${PROJECT_NAME}
  PRIVATE
  ${PYTHON_LIBRARIES}
  Python3::NumPy
)
