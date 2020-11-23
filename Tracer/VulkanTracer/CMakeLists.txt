cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(VulkanTracer)

add_library(${PROJECT_NAME} SHARED 
                        ${VulkanTracer_SRC_DIR}/src/Ray.cpp 

)