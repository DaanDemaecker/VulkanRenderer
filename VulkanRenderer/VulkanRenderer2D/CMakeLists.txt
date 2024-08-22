# Set project
project(VulkanRenderer2D)

# Create the executable
add_executable(VulkanRenderer2D
    "Engine/ConfigManager.cpp"
    "Engine/D2DEngine.cpp"
    "Engine/main.cpp"
    "Engine/Window.cpp"
    "Includes/STBIncludes.cpp"
    "Vulkan/Wrappers/GPUObject.cpp"
    "Vulkan/Wrappers/InstanceWrapper.cpp"
    "Vulkan/DispatchableManager.cpp"
 "Vulkan/Vulkan2D.cpp")

# Include directories specific to this target
target_include_directories(VulkanRenderer2D PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Add custom target to copy config file
add_custom_target(configFile2D ALL)
add_custom_command(
    TARGET configFile2D POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/Config.json
    $<TARGET_FILE_DIR:VulkanRenderer2D>
)

# Copy resources after Shaders
add_custom_target(copy_resources_2D ALL)
add_custom_command(
    TARGET copy_resources_2D POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:VulkanRenderer2D>/resources
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        $<TARGET_FILE_DIR:VulkanRenderer2D>/resources
    MAIN_DEPENDENCY ${CMAKE_SOURCE_DIR}/resources
    DEPENDS Shaders
)
