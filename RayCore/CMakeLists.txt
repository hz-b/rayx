cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(RayCore)

add_library(${PROJECT_NAME} SHARED 
                            src/RayCore/Application.cpp
                            src/RayCore/BeamLine/BeamLine.cpp
                            
                            )

target_compile_definitions(${PROJECT_NAME} PRIVATE RAY_BUILD_DLL)
target_compile_definitions(${PROJECT_NAME} PUBLIC RAY_PLATFORM_WINDOWS)

target_include_directories(${PROJECT_NAME} PRIVATE ${RayCore_SOURCE_DIR}/src/RayCore/)