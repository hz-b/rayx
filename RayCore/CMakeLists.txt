cmake_minimum_required(VERSION 3.18.3)
project(RayCore VERSION 0.0.1 DESCRIPTION "Core library of Ray-UI")


add_compile_definitions(RAY_BUILD_DLL RAY_PLATFORM_WINDOWS)

add_library(${PROJECT_NAME} SHARED 
                src/RayCore/Application.cpp
                src/RayCore/Controller.cpp
                src/RayCore/BeamLine/BeamLine.cpp
                src/RayCore/BeamLine/BeamLineObject.cpp
                src/RayCore/BeamLine/ImagePlane/ImagePlane.cpp
            )


target_include_directories(RayCore PRIVATE ${PROJECT_SOURCE_DIR}/src/RayCore)


set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)