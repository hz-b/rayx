cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

# ---- Add tests ----
# add_subdirectory(tests)
# -------------------


# ---- Project ----
project(rayx-ui)
set(CMAKE_CXX_STANDARD 20)

if(MSVC)
    add_compile_options(/W3 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

set(IMGUI_DIR ../../Extern/imgui/)
set(GLFW_DIR ../../Extern/glfw/)
set(GLM_DIR ../../Extern/glm/)
add_subdirectory(${GLFW_DIR} binary_dir EXCLUDE_FROM_ALL)

file(GLOB_RECURSE SOURCE ${PROJECT_SOURCE_DIR}/src/*.cpp)
set(IMGUI_SOURCES ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui_draw.cpp ${IMGUI_DIR}/imgui_demo.cpp ${IMGUI_DIR}/imgui_tables.cpp ${IMGUI_DIR}/imgui_widgets.cpp)
add_executable(${PROJECT_NAME} ${SOURCE} ${IMGUI_SOURCES})
# -----------------


# ---- GLFW Options ----
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)
# ----------------------


# ---- Dependencies ----
target_link_libraries(${PROJECT_NAME} PRIVATE rayx-core glfw)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${GLFW_DIR}/include
    ${GLFW_DIR}/deps
    ${IMGUI_DIR} 
    ${IMGUI_DIR}/backends 
    ${GLM_DIR}
    ..
)
# ----------------------