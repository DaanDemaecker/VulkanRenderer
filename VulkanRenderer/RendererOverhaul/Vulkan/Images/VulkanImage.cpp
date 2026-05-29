// VulkanImage.cpp

// Header include
#include "VulkanImage.h"

// File includes
#include "Vulkan/Core/VulkanCore.h"
#include "Vulkan/Core/VulkanAllocator.h"
#include "Vulkan/CommandBuffers/VulkanCommandPool.h"
#include "Vulkan/Images/STBImage.h"
#include "Vulkan/Buffers/VulkanBuffer.h"

#include "Vulkan/Barriers/VulkanPipelineBarrier.h"
#include "Vulkan/Barriers/VulkanImageBarrier.h"

// Standard library includes
#include <stdexcept>

DDM::VulkanImage::VulkanImage(const VulkanAllocator* pAllocator, const VulkanCore* pCore, const VulkanCommandPool* pCommandPool)
	:m_pAllocator{ pAllocator },
	m_pCore{ pCore },
	m_pCommandPool{ pCommandPool }
{

}

DDM::VulkanImage::~VulkanImage()
{
	if (m_Initialized)
	{
		vkDestroyImage(m_pCore->GetDeviceHandle(), m_VkImage, m_pAllocator->GetAllocator());
	}

	if (m_MemoryAllocated)
	{
		vkFreeMemory(m_pCore->GetDeviceHandle(), m_VkMemory, m_pAllocator->GetAllocator());
	}
}

void DDM::VulkanImage::FillBarrierInfo(VkImageMemoryBarrier& barrier)
{
	barrier.oldLayout = m_VkLayout;
	barrier.image = m_VkImage;

	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
}

void DDM::VulkanImage::FillCopyRegionInfo(VkBufferImageCopy& region)
{
	region.imageOffset = VkOffset3D{ 0,0,0 };
	region.imageExtent = m_VkExtent;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
}

void DDM::VulkanImage::AllocateMemory()
{
	// Create memory requirements object
	VkMemoryRequirements memRequirements;

	// Get memory requirements
	vkGetImageMemoryRequirements(m_pCore->GetDeviceHandle(), m_VkImage, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.pNext = nullptr;

	allocInfo.allocationSize = memRequirements.size;

	allocInfo.memoryTypeIndex = m_pCore->GetPhysicalDeviceInfo()->GetMemoryType(memRequirements);

	if (vkAllocateMemory(m_pCore->GetDeviceHandle(), &allocInfo, m_pAllocator->GetAllocator(), &m_VkMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate Vulkan Device memory");
	}

	vkBindImageMemory(m_pCore->GetDeviceHandle(), m_VkImage, m_VkMemory, 0);

	m_MemoryAllocated = true;
}