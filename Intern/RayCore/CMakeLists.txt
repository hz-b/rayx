cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(RayCore)

SET(SOURCE
    src/CanonicalizePath.cpp
    src/Random.cpp
    src/utils.cpp
    src/Data/Importer.cpp
    src/Data/xml.cpp
    src/Data/DatFile.cpp
    src/Debug/Debug.cpp
    src/Debug/Instrumentor.cpp
    src/Model/Beamline/Beamline.cpp
    src/Model/Beamline/OpticalElement.cpp
    src/Model/Beamline/LightSource.cpp
    src/Model/Beamline/EnergyDistribution.cpp
    src/Model/Beamline/Objects/ImagePlane.cpp
    src/Model/Beamline/Objects/MatrixSource.cpp
    src/Model/Beamline/Objects/PointSource.cpp
    src/Model/Beamline/Objects/Ellipsoid.cpp
    src/Model/Beamline/Objects/ReflectionZonePlate.cpp
    src/Model/Beamline/Objects/Slit.cpp
    src/Model/Beamline/Objects/SphereGrating.cpp
    src/Model/Beamline/Objects/PlaneGrating.cpp
    src/Model/Beamline/Objects/PlaneMirror.cpp
    src/Model/Beamline/Objects/PointSource.cpp
    src/Model/Beamline/Objects/SphereMirror.cpp
    src/Model/Beamline/Objects/ToroidMirror.cpp
    src/Model/Beamline/Objects/Cylinder.cpp
    src/Model/Beamline/Objects/Cone.cpp
    src/Model/Surface/Quadric.cpp
    src/Model/Surface/Toroid.cpp
    src/Model/Surface/Surface.cpp
    src/Material/Material.cpp
    src/Material/NffTable.cpp
    src/Material/PalikTable.cpp
    src/Tracer/Tracer.cpp
    src/Tracer/VulkanTracer.cpp
    src/Tracer/CpuTracer.cpp
    src/VulkanEngine/Cleanup.cpp
    src/VulkanEngine/DeclareBuffer.cpp
    src/VulkanEngine/CreateBuffer.cpp
    src/VulkanEngine/BufferIO/GpuMemcpy.cpp
    src/VulkanEngine/BufferIO/ReadBufferRaw.cpp
    src/VulkanEngine/BufferIO/WriteBufferRaw.cpp
    src/VulkanEngine/Destruct.cpp
    src/VulkanEngine/Init/CreateCommandPool.cpp
    src/VulkanEngine/Init/CreateDescriptorSetLayout.cpp
    src/VulkanEngine/Init/CreateInstance.cpp
    src/VulkanEngine/Init/CreateStagingBuffer.cpp
    src/VulkanEngine/Init/Init.cpp
    src/VulkanEngine/Init/PickDevice.cpp
    src/VulkanEngine/Run/CreateCommandBuffer.cpp
    src/VulkanEngine/Run/CreateComputePipeline.cpp
    src/VulkanEngine/Run/CreateDescriptorSet.cpp
    src/VulkanEngine/Run/RunCommandBuffer.cpp
    src/VulkanEngine/Run/Run.cpp
    src/Writer/CSVWriter.cpp
    src/Writer/H5Writer.cpp
    src/Plotter/Plotter.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCE})

target_precompile_headers(${PROJECT_NAME} PRIVATE raycorePCH.h)
set(CMAKE_PCH_INSTANTIATE_TEMPLATES ON)

# Set Compiler
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(COMPILE_PLATFORM RAYX_PLATFORM_GCC)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(COMPILE_PLATFORM RAYX_PLATFORM_MSVC)
else()
    set(COMPILE_PLATFORM UNKNOWN)
endif()

target_compile_definitions(${PROJECT_NAME}
    PRIVATE RAYX_BUILD_DLL
    PUBLIC ${COMPILE_PLATFORM}
    PROJECT_DIR="${CMAKE_SOURCE_DIR}"
    $<$<CONFIG:Debug>:RAYX_DEBUG_MODE>
    $<$<CONFIG:RelWithDebInfo>:RAYX_DEBUG_MODE>
)

find_package(Vulkan 1.3.216 REQUIRED)
target_link_libraries(${PROJECT_NAME}
    PUBLIC Vulkan::Vulkan
)

# Link H5 Format
if(NOT "$ENV{CI}" STREQUAL "YES")
    if(UNIX)
        target_include_directories(${PROJECT_NAME} PUBLIC /usr/include/hdf5/serial)
        target_link_directories(${PROJECT_NAME} PUBLIC /usr/lib/x86_64-linux-gnu/hdf5/serial)
        target_link_libraries(${PROJECT_NAME} PUBLIC hdf5)
    else()
        find_package(HDF5 REQUIRED)
        target_link_libraries(${PROJECT_NAME}
            PUBLIC HDF5::HDF5
        )
    endif()
endif()

# Add Extern deps TODO(Jannis): the src include shouldn't be a SYSTEM include
target_include_directories(${PROJECT_NAME}
    SYSTEM PUBLIC ${RayCore_SOURCE_DIR}/src
    PUBLIC ${RayCore_SOURCE_DIR}/../../Extern/glm/glm/
    PUBLIC ${RayCore_SOURCE_DIR}/../../Extern/rapidxml-1.13/
    PUBLIC ${RayCore_SOURCE_DIR}/../../Extern/HighFive/include/
    PUBLIC ${RayCore_SOURCE_DIR}/../../Extern/VMA/include/
)

# Compile Shaders
# The following code is used to always compile the shader.
# This is most likely not an optimal solution, but it will work
# until we find a better one.
add_dependencies(${PROJECT_NAME} COMPILE_SHADER)
set(COMPILED_SHADER ${CMAKE_BINARY_DIR}/bin/comp.spv)
set(COMPILED_SHADER_FAKE ${CMAKE_BINARY_DIR}/bin/___comp.spv) # this exists so file cannot be found -> always execute command

if(WIN32)
    add_custom_command(
        OUTPUT
        ${COMPILED_SHADER}
        ${COMPILED_SHADER_FAKE}
        COMMAND "$ENV{VULKAN_SDK}/Bin/glslc.exe"
        ARGS ${RayCore_SOURCE_DIR}/src/Tracer/shader/main-glsl.comp -o ${COMPILED_SHADER}
    )
elseif(UNIX)
    add_custom_command(
        OUTPUT
        ${COMPILED_SHADER}
        ${COMPILED_SHADER_FAKE}
        COMMAND "glslc"
        ARGS ${RayCore_SOURCE_DIR}/src/Tracer/shader/main-glsl.comp -o ${COMPILED_SHADER}
    )
endif()

add_custom_target(COMPILE_SHADER ALL DEPENDS ${COMPILED_SHADER})

# find python libraries
find_package(Python3 COMPONENTS Interpreter Development NumPy REQUIRED)
find_package(PythonLibs 3.0 REQUIRED)
target_include_directories(${PROJECT_NAME}
    PUBLIC ${RayCore_SOURCE_DIR}/../../Extern/matplotlib-cpp
    PUBLIC ${PYTHON3_INCLUDE_DIRS}
    PUBLIC ${NumPy_INCLUDE_DIRS}
)
target_link_libraries(${PROJECT_NAME}
    PUBLIC
    ${PYTHON_LIBRARIES}
    Python3::NumPy
)

# Print include directories
get_target_property(INCLUDE_DIRECTORIES ${PROJECT_NAME} INCLUDE_DIRECTORIES)
message(STATUS "Include directories for ${PROJECT_NAME}: ${INCLUDE_DIRECTORIES}")
