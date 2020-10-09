cmake_minimum_required(VERSION 3.18.3)
project(RayCore VERSION 0.0.1 DESCRIPTION "Core library of Ray-UI")


add_compile_definitions(RAY_BUILD_DLL RAY_PLATFORM_WINDOWS)

add_library(${PROJECT_NAME} SHARED src/RayCore/Application.cpp)

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)