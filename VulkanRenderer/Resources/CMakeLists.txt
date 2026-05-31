function(add_shaders TARGET_NAME)
    set(SHADER_SOURCE_FILES ${ARGN})
    
    list(LENGTH SHADER_SOURCE_FILES FILE_COUNT)
    if(FILE_COUNT EQUAL 0)
        message(WARNING "No shader files provided for target ${TARGET_NAME}")
        return()
    endif()

    set(SHADER_COMMANDS)
    set(SHADER_PRODUCTS)

    foreach(SHADER_SOURCE IN LISTS SHADER_SOURCE_FILES)
        cmake_path(ABSOLUTE_PATH SHADER_SOURCE NORMALIZE)
        cmake_path(GET SHADER_SOURCE FILENAME SHADER_NAME)

        file(RELATIVE_PATH NEW_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${SHADER_SOURCE})

        set(NEW_PATH "${CMAKE_CURRENT_BINARY_DIR}/../${TARGET_NAME}/resources/${NEW_PATH}.spv")

        message(WARNING "${SHADER_SOURCE} -> ${NEW_PATH}")

        # build command
        list(APPEND SHADER_COMMANDS COMMAND
        ${Vulkan_GLSLC_EXECUTABLE}
        "${SHADER_SOURCE}"
        "-o"
        "${NEW_PATH}")

        list(APPEND SHADER_PRODUCTS "${NEW_PATH}")

        endforeach()

     add_custom_target(Compile_Shaders_${TARGET_NAME} ALL
     ${SHADER_COMMANDS}
     COMMENT "Compiling shaders [${ARGN}]"
     SOURCES ${SHADER_SOURCE_FILES}
     BYPRODUCTS ${SHADER_PRODUCTS}
     )

endfunction()

file(GLOB_RECURSE SHADERS "*.frag" "*.vert")

add_shaders(RendererOverhaul
    ${SHADERS} 
)

add_shaders(VulkanRenderer3D
    ${SHADERS} 
)