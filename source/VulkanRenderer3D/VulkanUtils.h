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

	D3D::SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

	D3D::QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

	// Check if a certain extension is supported
		// Parameters:
		//     extensionName: the name of the extension that should be checked
	bool CheckExtensionSupport(const char* extensionName);
}

#endif // !VulkanUtilsIncluded