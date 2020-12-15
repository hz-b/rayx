cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(VulkanTracer)

# OS specific
if (WIN32)
    execute_process(COMMAND $ENV{VULKAN_SDK}/Bin/glslc.exe ${VulkanTracer_SOURCE_DIR}/src/shader.comp -o ${CMAKE_BINARY_DIR}/bin/comp.spv)
elseif (UNIX)
    set_property(TARGET lib1 PROPERTY POSITION_INDEPENDENT_CODE ON)
endif

find_package(Vulkan REQUIRED FATAL_ERROR)

add_library(${PROJECT_NAME} 
    STATIC 
        src/Ray.cpp 
        src/Ray.h 
        src/VulkanTracer.cpp
        src/VulkanTracer.h
)

target_include_directories(${PROJECT_NAME}  
    PUBLIC $ENV{VULKAN_SDK}/include
    PUBLIC ${VulkanTracer_SOURCE_DIR}/src/
)

target_link_libraries(${PROJECT_NAME}
    Vulkan::Vulkan
)