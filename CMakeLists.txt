cmake_minimum_required(VERSION 3.15 FATAL_ERROR)


# ---- Project ----
project(RAYX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# -----------------


# ---- Subdirectories ----
add_subdirectory(Intern)
add_subdirectory(Extern)
# ------------------------


# ---- Options ----
option(WERROR "add -Werror option" "NO") # inactive per default
option(VULKAN "include the VulkanTracer" "YES") # activated per default
option(H5 "include the H5Writer" "YES") # active per default
# ------------------


# ---- Code Coverage ----
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/Extern/cmake)
# -----------------------
