cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

# ---- Code Coverage ----
option(BUILD_WITH_COV "Add Code Coverage to build" TRUE)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_BUILD_TYPE STREQUAL "Debug" AND BUILD_WITH_COV)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
    message("Building with llvm-cov support")
endif()

# -----------------------

# ---- Add tests ----
# TODO re-enable
 add_subdirectory(tests)

# -------------------

# ---- Project ----
project(rayx-core VERSION ${RAYX_VERSION})
file(GLOB_RECURSE SOURCE ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_library(${PROJECT_NAME} SHARED ${SOURCE})

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static)

# -----------------


# -----OpenMP--------
find_package(OpenMP)

if(OpenMP_CXX_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX)
else()
    message(STATUS "OpenMP not found")
    target_compile_definitions(${PROJECT_NAME} PUBLIC NO_OMP)
endif()

# -------------------

# ---- Warnings ----
if(MSVC)
    message(STATUS "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE /W3 /wd4251 /wd4267)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    message(STATUS "GCC")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic) # -Werror)

    # Enable werror if requested (mostly used for CI)
    if(WERROR STREQUAL "YES")
        target_compile_options(${PROJECT_NAME} PRIVATE -O3 -Werror)
        message(STATUS "Werror mode")
    endif()
endif()

# ------------------

# ---- Disable Vulkan/H5 ----
find_package(Vulkan)

if(Vulkan_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC Vulkan::Vulkan)
else()
    message(STATUS "Vulkan not found")
    target_compile_definitions(${PROJECT_NAME} PUBLIC NO_VULKAN)
endif()

find_package(HDF5)

if(HDF5_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${HDF5_LIBRARIES})
    target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${HDF5_INCLUDE_DIRS})
else()
    message(STATUS "HDF5 not found")
    target_compile_definitions(${PROJECT_NAME} PUBLIC NO_H5)
endif()

# -------------------------

# ---- PCH ----
target_precompile_headers(${PROJECT_NAME} PUBLIC raycorePCH.h)
set(CMAKE_PCH_INSTANTIATE_TEMPLATES ON)

# -------------

# ---- Defines ----
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(COMPILE_PLATFORM RAYX_PLATFORM_GCC)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(COMPILE_PLATFORM RAYX_PLATFORM_MSVC)
else()
    set(COMPILE_PLATFORM RAYX_PLATFORM_UNKNOWN)
endif()

target_compile_definitions(${PROJECT_NAME}
    PRIVATE RAYX_BUILD_DLL
    PUBLIC ${COMPILE_PLATFORM}
    RAYX_PROJECT_DIR="${CMAKE_SOURCE_DIR}"
    $<$<CONFIG:Debug>:RAYX_DEBUG_MODE>
    $<$<CONFIG:RelWithDebInfo>:RAYX_DEBUG_MODE>
)

# -----------------

# ---- Dependencies ----
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} SYSTEM INTERFACE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC
    ${PROJECT_SOURCE_DIR}/../../Extern/glm/glm/
    ${PROJECT_SOURCE_DIR}/../../Extern/rapidxml-1.13/
    ${PROJECT_SOURCE_DIR}/../../Extern/HighFive/include/
    ${PROJECT_SOURCE_DIR}/../../Extern/VMA/include/
)

# ----------------------

# ---- Compile Shaders ----
if(Vulkan_FOUND)
    add_dependencies(${PROJECT_NAME} RAYX_CORE_COMPILE_SHADER)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Shaders)
    else()
        set(OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Shaders)
    endif()
    set(RAYX_CORE_SHADER ${OUTPUT_DIR}/comp.spv)

    file(MAKE_DIRECTORY ${OUTPUT_DIR})
    add_custom_command(
        OUTPUT
        ${RAYX_CORE_SHADER}
        COMMAND glslangValidator
        ARGS -V ${PROJECT_SOURCE_DIR}/src/Shader/main-glsl.comp -o ${RAYX_CORE_SHADER}
    )
    add_custom_target(RAYX_CORE_COMPILE_SHADER ALL DEPENDS ${RAYX_CORE_SHADER})
endif()

# ------------------------

# ---- Data ----
# Define the source and destination paths
set(DATA_SRC_DIR "${CMAKE_SOURCE_DIR}/Data")
# Set the destination directory for the Data directory based on the build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(DATA_DST_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Data")
else()
    set(DATA_DST_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Data")
endif()

# Copy the Data directory to the binary output directory after building
message(STATUS "Copying Data directory from ${DATA_SRC_DIR} to ${DATA_DST_DIR}")
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${DATA_SRC_DIR} ${DATA_DST_DIR}
    COMMENT "Copying Data directory to build output directory..."
)
# -----------------

# ---- Scripts ----
# Define the source and destination paths
set(SCRIPT_SRC_DIR "${CMAKE_SOURCE_DIR}/Scripts/plot.py")
# Set the destination directory for the Scripts directory based on the build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(SCRIPT_DST_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Scripts/plot.py")
else()
    set(SCRIPT_DST_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Scripts/plot.py")
endif()

# Copy the Scripts directory to the binary output directory after building
message(STATUS "Copying Scripts directory from ${SCRIPT_SRC_DIR} to ${SCRIPT_DST_DIR}")
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${SCRIPT_SRC_DIR} ${SCRIPT_DST_DIR}
    COMMENT "Copying Scripts directory to build output directory..."
)
# -----------------

# ---- CPack ----
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_CONTACT "Your Name <your.email@example.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Your Project Description")
install(FILES ${SCRIPT_SRC_DIR} DESTINATION bin/Scripts)

include(CPack)

# -----------------