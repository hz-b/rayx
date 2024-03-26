cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

# ---- Build options ----
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/release)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/debug)

# -----------------------

# ---- Subdirectories ----
add_subdirectory(rayx-core)
add_subdirectory(rayx)

# Don't build rayx-ui without Vulkan
if(NOT VULKAN STREQUAL "NO")
     add_subdirectory(rayx-ui)
endif()

# ------------------------
