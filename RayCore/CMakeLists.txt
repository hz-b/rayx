cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(RayCore)

add_library(${PROJECT_NAME} SHARED 
                            src/RayCore/Application.cpp
                            src/RayCore/BeamLine/BeamLine.cpp
                            src/RayCore/Tracer/Ray.cpp
                            src/RayCore/Tracer/TracerInterface.cpp
                            )

target_compile_definitions(${PROJECT_NAME} PRIVATE RAY_BUILD_DLL)
target_compile_definitions(${PROJECT_NAME} PUBLIC RAY_PLATFORM_WINDOWS)

target_include_directories(${PROJECT_NAME}  PUBLIC ${RayCore_SOURCE_DIR}/src/RayCore/
                                            PUBLIC ${RayCore_SOURCE_DIR}/vendor/glm/glm/)
                                            
target_link_libraries(${PROJECT_NAME} VulkanTracer)


set_target_properties(${PROJECT_NAME} PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY_DEBUG ${TerminalApp_BINARY_DIR}/
                      RUNTIME_OUTPUT_DIRECTORY_RELEASE ${TerminalApp_BINARY_DIR}/
)