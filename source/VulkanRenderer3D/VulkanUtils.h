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
	// Create a VkBuffer
	// Parameters:
	//     device: handle of the VkDevice
	//     physicalDevice: handle of the VkPhysicalDevice
	//     size: the size of the buffer
	//     usage: the usage flags for the buffer
	//     properties: the property flags for the buffer
	//     buffer: a reference to the buffer that will be created
	//     bufferMemory: a reference to the memory for the buffer that will be created
	void CreateBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size,
		VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	// Get a memory type that satisfies the given properties
	// Parameters:
	//     physicalDevice: handle of the VkPhysicalDevice
	//     typeFilter: specifies the bit field of the requested memory type
	//     properties: the requested properties for the memory type
	uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

	// Get the format for the depth resources
	// Parameters:
	//     physicalDevice: handle of the VkPhysicalDevice
	VkFormat FindDepthFormat(VkPhysicalDevice physicalDevice);

	// Get the maximum amount of samples per pixel
	// Parameters:
	//     physicalDevice: handle of the VkPhysicalDevice
	VkSampleCountFlagBits GetMaxUsableSampleCount(VkPhysicalDevice physicalDevice);

	// Find a supported format
	// Parameters:
	//     physicalDevice: handle of the VkPhysicalDevice
	//     candidates: the possible format candidates
	//     tiling: the tiling needed for the format
	//     features: the feature flags for the format
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