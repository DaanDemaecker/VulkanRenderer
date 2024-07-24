// VulkanUtils.h
// This file will hold some util functions for vulkan

#ifndef VulkanUtilsIncluded
#define VulkanUtilsIncluded

// File includes
#include "Includes/VulkanIncludes.h"
#include "DataTypes/Structs.h"

// Standard library includes
#include <vector>

// Class forward declarations
namespace D3D
{
	class ImageManager;
	class GPUObject;
}

namespace VulkanUtils
{
	// Get a memory type that satisfies the given properties
	// Parameters:
	//     typeFilter: specifies the bit field of the requested memory type
	//     properties: the requested properties for the memory type
	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	// Get the format for the depth resources
	VkFormat FindDepthFormat();

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

	// Get necesarry detail for swapchainsupport
		// Parameters:
		//     physicalDevice: handle of the VkPhysicalDevice
		//     surface: handle of the VkSurfaceKHR
	D3D::SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

	// Create a depth image
		// Parameters:
		//     texture: reference to the texture that stores the depth image
		//     pGPUObject: pointer to the GPU Object
		//     samples: max amount of samples per pixel
		//     swapchainExtent: the extent of the swapchain, for use of the image size
		//     pImageManager: pointer to the image manager
	    //     commandBuffer: a one time use commandbuffer used to create the image
		//     sambleBitSet: a bool that indicates if this depth image will be used for sampling
	void CreateDepthImage(D3D::Texture& texture, D3D::GPUObject* pGPUObject, VkSampleCountFlagBits samples, VkExtent2D swapchainExtent,
		D3D::ImageManager* pImageManager, VkCommandBuffer commandBuffer, bool sampleBitSet = false);
}
#endif // !VulkanUtilsIncluded