# Create the executable
add_executable(VulkanRenderer2D
    "Engine Files/ConfigManager.cpp"
    "Engine Files/D2DEngine.cpp"
    "Engine Files/main.cpp"
    "Engine Files/Window.cpp"
    "Includes/STBIncludes.cpp"
    "Vulkan/Wrappers/GPUObject.cpp"
    "Vulkan/Wrappers/InstanceWrapper.cpp"
    "Vulkan/VulkanRenderer2D.cpp"
)

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