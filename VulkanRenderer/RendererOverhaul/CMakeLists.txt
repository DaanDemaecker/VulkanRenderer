
add_executable(RendererOverhaul 

"main.cpp"

"Includes/STBIncludes.cpp"

 "Engine/ConfigManager.cpp"
 "Engine/DDMEngine.cpp"

 "Engine/Window/Window.cpp"

 "ServiceLocator/ServiceLocator.cpp"

 "ServiceLocator/FileSystem/DDM/DDMFileSystem.cpp"

 "ServiceLocator/Renderer/VulkanRenderer/VulkanRenderer.cpp"
 
 "Vulkan/VulkanImplementation.cpp"

 "Vulkan/Core/VulkanCore.cpp"
 "Vulkan/Core/VulkanAllocator.cpp"
 "Vulkan/Core/PhysicalDeviceInfo.cpp"

 "Vulkan/Queues/VulkanQueue.cpp"
 "Vulkan/Queues/VulkanQueueFamily.cpp"

 "Vulkan/CommandBuffers/CommandPool.cpp"
 "Vulkan/CommandBuffers/CommandBuffer.cpp"
 
 "Vulkan/Images/STBImage.cpp"
 "Vulkan/Images/VulkanImage.cpp"

 "Vulkan/Buffers/VulkanBuffer.cpp"
 
 "Vulkan/Barriers/VulkanPipelineBarrier.cpp"
 "Vulkan/Barriers/VulkanImageBarrier.cpp")

# Include directories specific to this target
target_include_directories(RendererOverhaul PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Copy resources after Shaders
add_custom_target(copy_resources_overhaul ALL)

add_custom_command(
    TARGET copy_resources_overhaul POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:RendererOverhaul>/resources
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        $<TARGET_FILE_DIR:RendererOverhaul>/resources
    MAIN_DEPENDENCY ${CMAKE_SOURCE_DIR}/resources
    DEPENDS Shaders
)

# Add custom target to copy config file
add_custom_target(configFileOverhaul ALL)
add_custom_command(
    TARGET configFileOverhaul POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/Config.json
    $<TARGET_FILE_DIR:RendererOverhaul>
)