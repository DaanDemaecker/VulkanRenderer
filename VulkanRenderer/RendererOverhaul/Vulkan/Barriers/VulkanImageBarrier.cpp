// VulkanImageBarrier.cpp

// Header include
#include "VulkanImageBarrier.h"

// File includes
#include "Vulkan/Images/VulkanImage.h"

DDM::Vulkan::VulkanImageBarrier::VulkanImageBarrier(VulkanImage* pImage, VkImageLayout newLayout)
{
	m_pImage = pImage;

	m_VkImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	m_VkImageMemoryBarrier.pNext = nullptr;

	m_VkImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	m_VkImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	m_VkImageMemoryBarrier.newLayout = newLayout;

	m_pImage->FillBarrierInfo(m_VkImageMemoryBarrier);
}

DDM::Vulkan::VulkanImageBarrier::~VulkanImageBarrier()
{

}

void DDM::Vulkan::VulkanImageBarrier::Executed()
{
	m_pImage->SetLayout(m_VkImageMemoryBarrier.newLayout);
}