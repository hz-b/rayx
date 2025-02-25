cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

# -----------------------

# ---- Enable languages that we need ----
if(alpaka_ACC_GPU_CUDA_ENABLE)
    enable_language(CUDA)
endif()

# -----------------------

# ---- Subdirectories ----
add_subdirectory(rayx-core)
add_subdirectory(rayx)

# Don't build rayx-ui without Vulkan
if(NOT VULKAN STREQUAL "NO")
    add_subdirectory(rayx-ui)
endif()

# ------------------------
