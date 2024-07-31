cmake_minimum_required(VERSION 3.25.2 FATAL_ERROR)

# ---- Project ----
project(RAYX VERSION 0.20.0)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_STANDARD 20)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

# ------------------

# ---- Options ----
option(WERROR "add -Werror option" "NO") # inactive per default
option(RAYX_ENABLE_CUDA "This option enables the search for CUDA. Project will be compiled without cuda if not found." ON)
option(RAYX_REQUIRE_CUDA "If option 'RAYX_ENABLE_CUDA' is ON, this option will add the requirement that cuda must be found." OFF)

# ------------------

# ---- Subdirectories ----
add_subdirectory(Extern)
add_subdirectory(Intern)

# ---- Code Coverage ----
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/Extern/cmake)

# ------------------

# ---- CPack ----
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "RAYX")
set(CPACK_PACKAGE_CONTACT "Max Mustermann <" ${PROJECT_NAME} "@helmholtz-berlin.de>")
include(CPack)
# ------------------
