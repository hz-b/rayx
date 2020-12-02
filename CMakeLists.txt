cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(RayReworked)

add_subdirectory(RayCore)
add_subdirectory(TerminalApp)
add_subdirectory(Tracer)



get_target_property(raycore_basename RayCore OUTPUT_NAME)
get_target_property(raycore_suffix RayCore SUFFIX)
set(raycore_filename ${raycore_basename}${raycore_suffix})
add_custom_command(
    OUTPUT ${TerminalApp_BINARY_DIR}/${raycore_filename}
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:RayCore> ${TerminalApp_BINARY_DIR}
    DEPENDS RayCore
)
add_custom_target(copy_files_terminal ALL
    DEPENDS ${TerminalApp_BINARY_DIR}/${raycore_filename}
)
