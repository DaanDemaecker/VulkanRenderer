// VulkanSwapchainImage.cpp

// Header include
#include "VulkanSwapchainImage.h"

DDM::Vulkan::VulkanSwapchainImage::~VulkanSwapchainImage()
{

}

DDM::Vulkan::VulkanSwapchainImage::VulkanSwapchainImage(const VulkanAllocator* pAllocator, const VulkanCore* pCore, VkImage image)
	: VulkanImage(pAllocator, pCore, nullptr)
{
	m_VkImage = image;
}

std::unique_ptr<DDM::Vulkan::VulkanSwapchainImage> DDM::Vulkan::VulkanSwapchainImage::Create(const VulkanAllocator* pAllocator, const VulkanCore* pCore, VkImage image)
{
	return std::unique_ptr<VulkanSwapchainImage>(new VulkanSwapchainImage(pAllocator, pCore, image));
}
