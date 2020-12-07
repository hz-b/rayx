cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(VulkanTracer)

find_package(Vulkan REQUIRED FATAL_ERROR)

add_library(${PROJECT_NAME} STATIC 
                        src/Ray.cpp 
                        src/VulkanTracer.cpp
)

target_include_directories(${PROJECT_NAME}  PUBLIC ${Vulkan_INCLUDE_DIRS}
                                            PUBLIC ${VulkanTracer_SOURCE_DIR}/src/)