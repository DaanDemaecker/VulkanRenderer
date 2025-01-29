// ImageViewManager.cpp

// File includes
#include "ImageViewManager.h"
#include "ImageManager.h"
#include "Vulkan/VulkanUtils.h"
#include "Vulkan/Wrappers/GPUObject.h"


DDM3::ImageViewManager::ImageViewManager(VkSampleCountFlagBits msaaSamples)
	// Initialize max amount of samples per pixel
	:m_MsaaSamples{msaaSamples}
{
}

void DDM3::ImageViewManager::Cleanup(VkDevice device)
{
	// Clean up color image
	m_ColorImage.Cleanup(device);
	// Clean up depth image
	m_DepthImage.Cleanup(device);
}

void DDM3::ImageViewManager::CreateColorResources(GPUObject* pGPUObject, VkFormat format, VkExtent2D swapchainExtent, DDM3::ImageManager* pImageManager)
{
	// Create the image for the color
	// Set tiling to optimal
	// Set usage to transient attachment and color attachment
	// Set properties to device local
	pImageManager->CreateImage(pGPUObject, swapchainExtent.width, swapchainExtent.height, 1, m_MsaaSamples, format,
		VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_ColorImage);

	// Create image view for the color image
	m_ColorImage.imageView = pImageManager->CreateImageView(pGPUObject->GetDevice(), m_ColorImage.image, format, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

void DDM3::ImageViewManager::CreateDepthResources(GPUObject* pGPUObject, VkExtent2D swapchainExtent, DDM3::ImageManager* pImageManager, VkCommandBuffer commandBuffer)
{
	VulkanUtils::CreateDepthImage(m_DepthImage, pGPUObject, m_MsaaSamples, swapchainExtent, pImageManager, commandBuffer);
}