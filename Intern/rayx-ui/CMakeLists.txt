cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

find_package(Vulkan) # TODO: rayx-ui gets vulkan from rayx-core, this is a hack to get Vulkan_FOUND variable

if(Vulkan_FOUND)
    # ---- Add tests ----
    add_subdirectory(tests)
    # -------------------

    # ---- Project ----
    project(rayx-ui VERSION ${RAYX_VERSION})

    set(IMGUI_DIR ../../Extern/imgui/)
    set(GLFW_DIR ../../Extern/glfw/)
    set(GLM_DIR ../../Extern/glm/)
    set(STB_DIR ../../Extern/stb/)
    add_subdirectory(${GLFW_DIR} binary_dir EXCLUDE_FROM_ALL)

    file(GLOB_RECURSE SOURCE ${PROJECT_SOURCE_DIR}/src/*.cpp)
    set(IMGUI_SOURCES ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui_draw.cpp ${IMGUI_DIR}/imgui_demo.cpp ${IMGUI_DIR}/imgui_tables.cpp ${IMGUI_DIR}/imgui_widgets.cpp)

    add_executable(${PROJECT_NAME} ${SOURCE} ${IMGUI_SOURCES})

    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /W4)
        if(WERROR STREQUAL "YES")
            target_compile_options(${PROJECT_NAME} PRIVATE -WX)
            message(STATUS "Werror mode")
        endif()
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
        if(WERROR STREQUAL "YES")
            target_compile_options(${PROJECT_NAME} PRIVATE -Werror)
            message(STATUS "Werror mode")
        endif()
    endif()

    target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        GLM_FORCE_RADIANS
        GLM_FORCE_DEPTH_ZERO_TO_ONE)

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(RESOURCE_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Assets)
    else()
        set(RESOURCE_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Assets)
    endif()

    message(STATUS "Resource directory: ${RESOURCE_DIR}")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/res
        ${RESOURCE_DIR}
    )
    # -----------------


    # ---- CPack ----
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Shaders
                DESTINATION ${INSTALL_DATA_DIR}
                FILES_MATCHING PATTERN "*_*.spv")
        install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Assets
                DESTINATION ${INSTALL_DATA_DIR}
                PATTERN "fonts" EXCLUDE)
        install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Assets/fonts
                DESTINATION ${INSTALL_FONTS_DIR}
                FILES_MATCHING PATTERN "*")
    else()
        install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Shaders
                DESTINATION ${INSTALL_DATA_DIR}
                FILES_MATCHING PATTERN "*_*.spv")
        install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Assets
                DESTINATION ${INSTALL_DATA_DIR}
                PATTERN "fonts" EXCLUDE)
        install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Assets/fonts
                DESTINATION ${INSTALL_FONTS_DIR})
    endif()

    include(InstallRequiredSystemLibraries)
    include(CPack)
    # -----------------

    # ---- GLFW Options ----
    option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
    option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
    option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
    option(GLFW_INSTALL "Generate installation target" OFF)
    option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)
    # ----------------------

    # ---- Dependencies ----
    target_link_libraries(${PROJECT_NAME} PRIVATE rayx-core glfw CLI11::CLI11 Vulkan::Vulkan portable_file_dialogs)
    target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE 
        ${GLFW_DIR}/include
        ${GLFW_DIR}/deps
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
        ${GLM_DIR}
        ${STB_DIR}
        ..
    )
    # ----------------------

    # ---- Compile Shaders ----
    # Function to compile shaders
    function(compile_shaders target shaders_dir)
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Shaders)
        else()
            set(OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Shaders)
        endif()
        file(MAKE_DIRECTORY ${OUTPUT_DIR})
        # Get a list of all shader files in the specified directory
        file(GLOB SHADER_FILES "${shaders_dir}/*.frag" "${shaders_dir}/*.vert")

        foreach(SHADER_FILE ${SHADER_FILES})
            # Get the file name without the directory and extension
            get_filename_component(SHADER_NAME ${SHADER_FILE} NAME_WE)

            # Determine shader stage based on file extension
            get_filename_component(SHADER_EXT ${SHADER_FILE} EXT)
            string(SUBSTRING ${SHADER_EXT} 1 -1 SHADER_STAGE) # Remove the leading '.' from the extension

            # Set output file name
            set(OUTPUT_FILE ${OUTPUT_DIR}/${SHADER_NAME}_${SHADER_STAGE}.spv)

            # Create a custom command for each shader file
            add_custom_command(
                OUTPUT ${OUTPUT_FILE}
                COMMAND glslangValidator 
                ARGS -V ${SHADER_FILE} -o ${OUTPUT_FILE}
                DEPENDS ${SHADER_FILE}
                COMMENT "Compiling ${SHADER_FILE} to ${OUTPUT_FILE}"
            )

            # Add the output file to the list of dependencies for the custom target
            list(APPEND SHADER_OUTPUTS ${OUTPUT_FILE})
        endforeach()

        # Create a custom target to trigger the compilation of all shaders
        add_custom_target(${target} ALL DEPENDS ${SHADER_OUTPUTS})
    endfunction()

    # Call the function to compile all shaders in the specified directory
    compile_shaders(RAYX_UI_COMPILE_SHADER "${PROJECT_SOURCE_DIR}/src/Shaders")
    # ------------------------
endif() # Vulkan_FOUND
