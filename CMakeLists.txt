cmake_minimum_required(VERSION 3.18)
project(RayReworked VERSION 0.0.1)

# Remove 'lib' prefix for shared libraries on Windows
if (WIN32)
  set(CMAKE_SHARED_LIBRARY_PREFIX "")
  add_compile_definitions(RAY_PLATFORM_WINDOWS)
endif ()

add_subdirectory(RayCore)
add_subdirectory(GUIApp)
add_subdirectory(TerminalApp)


# ------------- Post Build Copy Command -------------
# Get filename for RayCore project
get_target_property(raycore_basename RayCore OUTPUT_NAME)
get_target_property(raycore_suffix RayCore SUFFIX)
set(raycore_filename ${raycore_basename}${raycore_suffix})

add_custom_command(
    OUTPUT ${GUIApp_BINARY_DIR}/${raycore_filename}
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:RayCore> ${GUIApp_BINARY_DIR}
    DEPENDS RayCore
)
add_custom_target(copy_files_gui ALL
    DEPENDS ${GUIApp_BINARY_DIR}/${raycore_filename}
)

add_custom_command(
    OUTPUT ${TerminalApp_BINARY_DIR}/${raycore_filename}
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:RayCore> ${TerminalApp_BINARY_DIR}
    DEPENDS RayCore
)
add_custom_target(copy_files_terminal ALL
    DEPENDS ${TerminalApp_BINARY_DIR}/${raycore_filename}
)