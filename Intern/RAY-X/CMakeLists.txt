cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

# ---- Add tests ----
# add_subdirectory(tests)
# -------------------


# ---- Project ----
project(RAY-X)
file(GLOB_RECURSE SOURCE ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCE})
# -----------------


# ---- Dependencies ----
target_link_libraries(${PROJECT_NAME} PRIVATE RAY-Core)