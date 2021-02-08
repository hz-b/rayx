cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(RayCore)

SET(SOURCE
    src/RayCore/Application.cpp
    src/RayCore/BeamLine/BeamLine.cpp
    src/RayCore/BeamLine/LightSource.cpp
    src/RayCore/BeamLine/MatrixSource.cpp
    src/RayCore/BeamLine/RandomRays.cpp
    src/RayCore/Beamline/Quadric.cpp
    src/RayCore/Beamline/SphereGrating.cpp
    src/RayCore/Beamline/PlaneGrating.cpp
    src/RayCore/Beamline/PlaneMirror.cpp
    src/RayCore/Beamline/SphereMirror.cpp
    src/RayCore/Beamline/utils.cpp
    src/RayCore/Tracer/Ray.cpp
    src/RayCore/Tracer/TracerInterface.cpp
)

SET(HEADERS
    src/RayCore/Application.h
    src/RayCore/Debug.h
    src/RayCore/BeamLine/BeamLine.h
    src/RayCore/BeamLine/BeamLineObject.h
    src/RayCore/BeamLine/LightSource.h
    src/RayCore/BeamLine/MatrixSource.h
    src/RayCore/BeamLine/RandomRays.h
    src/RayCore/Beamline/Quadric.h
    src/RayCore/Beamline/SphereGrating.h
    src/RayCore/Beamline/PlaneGrating.h
    src/RayCore/Beamline/PlaneMirror.h
    src/RayCore/Beamline/SphereMirror.h
    src/RayCore/Beamline/utils.h
    src/RayCore/Tracer/Ray.h
    src/RayCore/Tracer/TracerInterface.h
)


add_library(${PROJECT_NAME} SHARED ${SOURCE} ${HEADERS} )

target_link_libraries(${PROJECT_NAME} 
    PUBLIC VulkanTracer
)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    set(COMPILE_PLATFORM RAY_PLATFORM_GCC)
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    set(COMPILE_PLATFORM RAY_PLATFORM_WINDOWS)
else() 
    set(COMPILE_PLATFORM UNKNOWN)
endif()

target_compile_definitions(${PROJECT_NAME} 
    PRIVATE RAY_BUILD_DLL
    PUBLIC ${COMPILE_PLATFORM}
)

target_include_directories(${PROJECT_NAME}  
    PUBLIC ${RayCore_SOURCE_DIR}/src/RayCore/
    PUBLIC ${RayCore_SOURCE_DIR}/vendor/glm/glm/
)


# set_target_properties(${PROJECT_NAME} PROPERTIES
#                       RUNTIME_OUTPUT_DIRECTORY ${TerminalApp_BINARY_DIR}
# )