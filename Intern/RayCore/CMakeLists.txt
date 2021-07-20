cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(RayCore)

SET(SOURCE
    src/RayCore/Application.cpp
    src/RayCore/utils.cpp
    src/RayCore/Beamline/Beamline.cpp
    src/RayCore/Beamline/BeamlineObject.cpp
    src/RayCore/Beamline/OpticalElement.cpp
    src/RayCore/Beamline/LightSource.cpp
    src/RayCore/Beamline/BeamlineObjectFactory.cpp
    src/RayCore/Beamline/Objects/ImagePlane.cpp
    src/RayCore/Beamline/Objects/MatrixSource.cpp
    src/RayCore/Beamline/Objects/RandomRays.cpp
    src/RayCore/Beamline/Objects/PointSource.cpp
    src/RayCore/Beamline/Objects/Ellipsoid.cpp
    src/RayCore/Beamline/Objects/ReflectionZonePlate.cpp
    src/RayCore/Beamline/Objects/Slit.cpp
    src/RayCore/Beamline/Objects/SphereGrating.cpp
    src/RayCore/Beamline/Objects/PlaneGrating.cpp
    src/RayCore/Beamline/Objects/PlaneMirror.cpp
    src/RayCore/Beamline/Objects/PointSource.cpp
    src/RayCore/Beamline/Objects/SphereMirror.cpp
    src/RayCore/Data/Exporter.cpp
    src/RayCore/Data/Importer.cpp
    src/RayCore/Surface/Quadric.cpp
    src/RayCore/Surface/Surface.cpp
    src/RayCore/Tracer/Ray.cpp
    src/RayCore/Tracer/TracerInterface.cpp
)

SET(HEADERS
    src/RayCore/Application.h
    src/RayCore/Debug.h
    src/RayCore/utils.h
    src/RayCore/Beamline/Beamline.h
    src/RayCore/Beamline/BeamlineObject.h
    src/RayCore/Beamline/OpticalElement.h
    src/RayCore/Beamline/LightSource.h
    src/RayCore/Beamline/BeamlineObjectFactory.h
    src/RayCore/Beamline/Objects/ImagePlane.h
    src/RayCore/Beamline/Objects/MatrixSource.h
    src/RayCore/Beamline/Objects/RandomRays.h
    src/RayCore/Beamline/Objects/PointSource.h
    src/RayCore/Beamline/Objects/Ellipsoid.h
    src/RayCore/Beamline/Objects/ReflectionZonePlate.h
    src/RayCore/Beamline/Objects/Slit.h
    src/RayCore/Beamline/Objects/SphereGrating.h
    src/RayCore/Beamline/Objects/PlaneGrating.h
    src/RayCore/Beamline/Objects/PlaneMirror.h
    src/RayCore/Beamline/Objects/PointSource.h
    src/RayCore/Beamline/Objects/SphereMirror.h
    src/RayCore/Data/Exporter.h
    src/RayCore/Data/Importer.h
    src/RayCore/Surface/Quadric.h
    src/RayCore/Surface/Surface.h
    src/RayCore/Tracer/Ray.h
    src/RayCore/Tracer/TracerInterface.h
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
    set(COMPILE_PLATFORM RAYX_PLATFORM_GCC)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(COMPILE_PLATFORM RAYX_PLATFORM_WINDOWS)
else() 
    set(COMPILE_PLATFORM UNKNOWN)
endif()

target_compile_definitions(${PROJECT_NAME} 
    PRIVATE RAYX_BUILD_DLL
    PUBLIC ${COMPILE_PLATFORM}
)

target_include_directories(${PROJECT_NAME}  
    PUBLIC ${RayCore_SOURCE_DIR}/src/RayCore/
    PUBLIC ${RayCore_SOURCE_DIR}/../../Extern/glm/glm/
    PUBLIC ${RayCore_SOURCE_DIR}/../../Extern/rapidxml-1.13/
)



# set_target_properties(${PROJECT_NAME} PROPERTIES
#                       RUNTIME_OUTPUT_DIRECTORY ${TerminalApp_BINARY_DIR}
# )