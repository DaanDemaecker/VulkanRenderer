// VulkanUtils.h
// This file will hold some util functions for vulkan

#ifndef VulkanUtilsIncluded
#define VulkanUtilsIncluded

// File includes
#include "VulkanIncludes.h"
#include "Structs.h"

// Standard library includes
#include <stdexcept>

namespace VulkanUtils
{
	void CreateBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size,
		VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

	VkFormat FindDepthFormat(VkPhysicalDevice physicalDevice);

	VkSampleCountFlagBits GetMaxUsableSampleCount(VkPhysicalDevice physicalDevice);


	VkFormat FindSupportedFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

	// Find the needed queue families
		// Parameters:
		//     physicalDevice: handle of the VkPhysicalDevice
		//     surface: handle of the VkSurfaceKHR
	D3D::QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

	// Check if a certain extension is supported
		// Parameters:
		//     extensionName: the name of the extension that should be checked
	bool CheckExtensionSupport(const char* extensionName);
}

#endif // !VulkanUtilsIncluded