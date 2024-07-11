# Find the required packages
find_package(Vulkan REQUIRED)

# Include Directories
include_directories(${Vulkan_INCLUDE_DIRS})

# Link Vulkan library
target_link_libraries(VulkanRenderer3D PRIVATE Vulkan::Vulkan)

target_link_libraries(VulkanRenderer2D PRIVATE Vulkan::Vulkan)