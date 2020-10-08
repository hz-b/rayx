cmake_minimum_required(VERSION 3.18.3)

project(RayCore VERSION 0.0.1 DESCRIPTION "Core library of Ray-UI")

add_compile_definitions(RAY_BUILD_DLL RAY_PLATFORM_WINDOWS)

add_library(${PROJECT_NAME} SHARED 
    src/RayCore/Application.cpp
)

set_target_properties(RayCore PROPERTIES VERSION ${PROJECT_VERSION})
# set_target_properties(mylib PROPERTIES SOVERSION 1)
set_target_properties(RayCore PROPERTIES PUBLIC_HEADER src/RayCore.h)

# set(${PROJECT_NAME}_DEFINITIONS RAY_PLATFORM_WINDOWS
#     CACHE INTERNAL "{PROJECT_NAME}: Definitions" FORCE)

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)