#include "ImageViewManager.h"
#include "ImageManager.h"


D3D::ImageViewManager::ImageViewManager(VkSampleCountFlagBits msaaSamples)
	:m_MsaaSamples{msaaSamples}
{
}

void D3D::ImageViewManager::Cleanup(VkDevice device)
{
	m_ColorImage.Cleanup(device);
	m_DepthImage.Cleanup(device);
}

void D3D::ImageViewManager::CreateColorResources(VkDevice device, VkPhysicalDevice physicalDevice, VkFormat format, VkExtent2D swapchainExtent, D3D::ImageManager* pImageManager)
{
	VkFormat colorFormat = format;

	pImageManager->CreateImage(device, physicalDevice, swapchainExtent.width, swapchainExtent.height, 1, m_MsaaSamples, colorFormat,
		VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_ColorImage);

	m_ColorImage.imageView = pImageManager->CreateImageView(device, m_ColorImage.image, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

void D3D::ImageViewManager::CreateDepthResources(VkDevice device, VkPhysicalDevice physicalDevice, VkFormat depthFormat, VkExtent2D swapchainExtent, D3D::ImageManager* pImageManager, VkCommandBuffer commandBuffer)
{
	pImageManager->CreateImage(device, physicalDevice, swapchainExtent.width, swapchainExtent.height, 1, m_MsaaSamples, depthFormat,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		m_DepthImage);

	m_DepthImage.imageView = pImageManager->CreateImageView(device, m_DepthImage.image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

	pImageManager->TransitionImageLayout(m_DepthImage.image, commandBuffer, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
}