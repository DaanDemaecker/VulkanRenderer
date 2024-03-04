// VulkanUtils.cpp

// File includes
#include "VulkanUtils.h"

// Standard library includes

void VulkanUtils::CreateBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size,
	VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	// Create buffer create info
	VkBufferCreateInfo bufferInfo{};
	// Set type to buffer create info
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	// Set size to given size
	bufferInfo.size = size;
	// Set usage to given usage
	bufferInfo.usage = usage;
	// Set sharing mode to exclusive
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	// Create buffer
	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to create buffer!");
	}

	// Create memory requiremnts object
	VkMemoryRequirements memRequirements;
	// Get the memory requirements
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	// Create memory allocation info
	VkMemoryAllocateInfo allocInfo{};
	// Set type to memory allocate info
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	// Set size to memory requirements size
	allocInfo.allocationSize = memRequirements.size;
	// Find a memory type that satisfies the requested properties
	allocInfo.memoryTypeIndex = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

	// Allocate the memory
	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	// Bind the buffer to the buffer memory
	vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

uint32_t VulkanUtils::FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	// Create physical device memory properties object
	VkPhysicalDeviceMemoryProperties memProperties;
	// Get the memory properties
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	// Loop trough the amount of memory types
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
	{
		// If the type filter coralates and the properties are the same
		if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			// Return the memory type
			return i;
		}
	}

	// If no memory type was found, throw runtime error
	throw std::runtime_error("failed to find suitable memory type!");
}

VkFormat VulkanUtils::FindDepthFormat(VkPhysicalDevice physicalDevice)
{
	// Find a supported format for the depth resources
	// Requested property flag is Depth stencil attachment
	return VulkanUtils::FindSupportedFormat(physicalDevice,
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

VkFormat VulkanUtils::FindSupportedFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	// Loop trough the candidates
	for (const VkFormat& format : candidates)
	{
		// Create format properties object
		VkFormatProperties props;
		// Get the format properites
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

		// If tiling mode is set to linear, compare the linear tiling features of the format to the requested features
		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
		{
			// If features satisfy the requested features, return the format
			return format;
		}
		// If tiling mode is set to optimal, compare the optimal tiling features of them format to the requested features
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
		{
			// If features satisfy the requested features, return the format
			return format;
		}
	}

	// If no format is returned, throw runtime error
	throw std::runtime_error("failed to find supported format!");
}

D3D::QueueFamilyIndices VulkanUtils::FindQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
	// Create quefamilyindices object
	D3D::QueueFamilyIndices indices;

	// Create uint for amount of queuefamilies
	uint32_t queueFamilyCount = 0;
	// Get the amount of queuefamilies
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
	// Create vector the size of the amount of queuefamilies
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	// Get the queuefamilies
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());


	// Loop trough the amount of queueFamilies
	for (uint32_t i{}; i < queueFamilies.size(); i++)
	{
		// If the queueflag cointains the queue graphics bit
		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			// Set graphics family of the indices to i
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		// Check if the graphics family index is supported
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

		// If it is supported
		if (presentSupport)
		{
			// Set the present family of indices to i
			indices.presentFamily = i;
		}

		// If indices is complete, jump out of the loop
		if (indices.isComplete())
			break;
	}

	// Return the found indices
	return indices;
}

bool VulkanUtils::CheckExtensionSupport(const char* extensionName)
{
	// Create uint for the extension count
	uint32_t extensionCount;
	// Get the amount of extensions
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	// Create vector of extension properties the size of the extension count
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	// Get the avalable extensions
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

	// Loop trough the available extensions
	for (const auto& extension : availableExtensions)
	{
		// If the name of the current extension is equal to the name of the requeste extension, return true
		if (strcmp(extension.extensionName, extensionName) == 0)
		{
			return true;
		}
	}

	// If no available extension was found, return false
	return false;
}

VkSampleCountFlagBits VulkanUtils::GetMaxUsableSampleCount(VkPhysicalDevice physicalDevice)
{
	// Create physical device properties object
	VkPhysicalDeviceProperties physicalDeviceProperties{};
	// Get physical device properties
	vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

	// Get the max amount of color and depth sample counts
	VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts &
		physicalDeviceProperties.limits.framebufferDepthSampleCounts;

	// If counts contains 64 bit, return 64 bit
	if (counts & VK_SAMPLE_COUNT_64_BIT)
	{
		return VK_SAMPLE_COUNT_64_BIT;
	}

	// If counts contains 32 bit, return 32 bit
	if (counts & VK_SAMPLE_COUNT_32_BIT)
	{
		return VK_SAMPLE_COUNT_32_BIT;
	}

	// If counts contains 16 bit, return 16 bit
	if (counts & VK_SAMPLE_COUNT_16_BIT)
	{
		return VK_SAMPLE_COUNT_16_BIT;
	}

	// If counts contains 8 bit, return 8 bit
	if (counts & VK_SAMPLE_COUNT_8_BIT)
	{
		return VK_SAMPLE_COUNT_8_BIT;
	}

	// If counts contains 4 bit, return 4 bit
	if (counts & VK_SAMPLE_COUNT_4_BIT)
	{
		return VK_SAMPLE_COUNT_4_BIT;
	}

	// If counts contains 2 bit, return 2 bit
	if (counts & VK_SAMPLE_COUNT_2_BIT)
	{
		return VK_SAMPLE_COUNT_2_BIT;
	}

	// If nothing was returned, return 1
	return VK_SAMPLE_COUNT_1_BIT;
}