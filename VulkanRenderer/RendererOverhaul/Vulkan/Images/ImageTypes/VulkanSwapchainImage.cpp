#include "VulkanSwapchainImage.h"

DDM::VulkanSwapchainImage::~VulkanSwapchainImage()
{

}

DDM::VulkanSwapchainImage::VulkanSwapchainImage(const VulkanAllocator* pAllocator, const VulkanCore* pCore, VkImage image)
	: VulkanImage(pAllocator, pCore, nullptr)
{
	m_VkImage = image;
}

std::unique_ptr<DDM::VulkanSwapchainImage> DDM::VulkanSwapchainImage::Create(const VulkanAllocator* pAllocator, const VulkanCore* pCore, VkImage image)
{
	return std::unique_ptr<VulkanSwapchainImage>(new VulkanSwapchainImage(pAllocator, pCore, image));
}
