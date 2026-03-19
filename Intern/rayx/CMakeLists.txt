cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

# if(alpaka_ACC_GPU_CUDA_ENABLE)
#     enable_language(CUDA)
# endif()

# ---- Project ----
project(rayx VERSION ${RAYX_VERSION})
file(GLOB_RECURSE SOURCE ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCE})
# ------------------


# ---- Dependencies ----
target_link_libraries(${PROJECT_NAME} PRIVATE
  CLI11::CLI11
  rayx-core
  ${PYTHON_LIBRARIES}
)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_BINARY_DIR})
# ----------------------

# ---- CPack ----
if(APPLE OR UNIX)
  install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
elseif(WIN32)
  install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION .)
endif()

include(InstallRequiredSystemLibraries)
include(CPack)
# --------------
