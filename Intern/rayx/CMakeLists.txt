cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

# if(alpaka_ACC_GPU_CUDA_ENABLE)
#     enable_language(CUDA)
# endif()

# ---- Project ----
project(rayx VERSION ${RAYX_VERSION})
file(GLOB_RECURSE SOURCE ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCE})
# ------------------


# ---- Git Info ----
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
# Config file for the project (e.g. git hash can be referenced in the code)
configure_file(TerminalAppConfig.h.in TerminalAppConfig.h)
# ------------------


# ---- Dependencies ----
target_link_libraries(${PROJECT_NAME} PRIVATE
  CLI11::CLI11
  rayx-core
  ${PYTHON_LIBRARIES}
)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_BINARY_DIR})
# ----------------------

# ---- CPack ----
if(APPLE OR UNIX)
  install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
elseif(WIN32)
  install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION .)
endif()

include(InstallRequiredSystemLibraries)
include(CPack)
# --------------
