# Source files
set(DEFAULT_SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(DEFAULT_SHADER_BINARY_DIR "${CMAKE_BINARY_DIR}/VulkanRenderer3D/resources/DefaultResources")

file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${DEFAULT_SHADER_SOURCE_DIR}/*.frag"
    "${DEFAULT_SHADER_SOURCE_DIR}/*.vert"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${DEFAULT_SHADER_BINARY_DIR}/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} -g ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL} Shaders
        POST_BUILD
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)


add_custom_target(
    DefaultShaders 
    DEPENDS ${SPIRV_BINARY_FILES}
)

add_dependencies(VulkanRenderer3D DefaultShaders)
