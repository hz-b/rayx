cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(RayCore)

SET(SOURCE
    src/RayCore/Application.cpp
    src/RayCore/Beamline/Beamline.cpp
    src/RayCore/Beamline/ImagePlane.cpp
    src/RayCore/Beamline/LightSource.cpp
    src/RayCore/Beamline/MatrixSource.cpp
    src/RayCore/Beamline/RandomRays.cpp
    src/RayCore/Beamline/PointSource.cpp
    src/RayCore/Beamline/Ellipsoid.cpp
    src/RayCore/Beamline/ReflectionZonePlate.cpp
    src/RayCore/Beamline/Slit.cpp
    src/RayCore/Beamline/Quadric.cpp
    src/RayCore/Beamline/SphereGrating.cpp
    src/RayCore/Beamline/PlaneGrating.cpp
    src/RayCore/Beamline/PlaneMirror.cpp
    src/RayCore/Beamline/PointSource.cpp
    src/RayCore/Beamline/SphereMirror.cpp
    src/RayCore/Beamline/utils.cpp
    src/RayCore/Tracer/Ray.cpp
    src/RayCore/Tracer/TracerInterface.cpp
    src/RayCore/Data/Exporter.cpp
)

SET(HEADERS
    src/RayCore/Application.h
    src/RayCore/Debug.h
    src/RayCore/Beamline/Beamline.h
    src/RayCore/Beamline/ImagePlane.h
    src/RayCore/Beamline/BeamlineObject.h
    src/RayCore/Beamline/LightSource.h
    src/RayCore/Beamline/MatrixSource.h
    src/RayCore/Beamline/RandomRays.h
    src/RayCore/Beamline/PointSource.h
    src/RayCore/Beamline/Ellipsoid.h
    src/RayCore/Beamline/ReflectionZonePlate.h
    src/RayCore/Beamline/Slit.h
    src/RayCore/Beamline/Quadric.h
    src/RayCore/Beamline/SphereGrating.h
    src/RayCore/Beamline/PlaneGrating.h
    src/RayCore/Beamline/PlaneMirror.h
    src/RayCore/Beamline/PointSource.h
    src/RayCore/Beamline/SphereMirror.h
    src/RayCore/Beamline/utils.h
    src/RayCore/Tracer/Ray.h
    src/RayCore/Tracer/TracerInterface.h
    src/RayCore/Data/Exporter.h
)


add_library(${PROJECT_NAME} SHARED ${SOURCE} ${HEADERS} )

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4) # /WX
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall)# -Wextra)# -pedantic) # -Werror
endif()

target_link_libraries(${PROJECT_NAME} 
    PUBLIC VulkanTracer
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(COMPILE_PLATFORM RAY_PLATFORM_GCC)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
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
    PUBLIC ${RayCore_SOURCE_DIR}/vendor/rapidxml-1.13/
)


# set_target_properties(${PROJECT_NAME} PROPERTIES
#                       RUNTIME_OUTPUT_DIRECTORY ${TerminalApp_BINARY_DIR}
# )