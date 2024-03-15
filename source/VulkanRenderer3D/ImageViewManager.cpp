// ImageViewManager.cpp

// File includes
#include "ImageViewManager.h"
#include "ImageManager.h"
#include "VulkanUtils.h"
#include "GPUObject.h"


D3D::ImageViewManager::ImageViewManager(VkSampleCountFlagBits msaaSamples)
	// Initialize max amount of samples per pixel
	:m_MsaaSamples{msaaSamples}
{
}

void D3D::ImageViewManager::Cleanup(VkDevice device)
{
	// Clean up color image
	m_ColorImage.Cleanup(device);
	// Clean up depth image
	m_DepthImage.Cleanup(device);
}

void D3D::ImageViewManager::CreateColorResources(GPUObject* pGPUObject, VkFormat format, VkExtent2D swapchainExtent, D3D::ImageManager* pImageManager)
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

void D3D::ImageViewManager::CreateDepthResources(GPUObject* pGPUObject, VkExtent2D swapchainExtent, D3D::ImageManager* pImageManager, VkCommandBuffer commandBuffer)
{
	// Get the depth formatµ
	auto depthFormat = VulkanUtils::FindDepthFormat(pGPUObject->GetPhysicalDevice());

	// Create the image for the depth
	// Set tiling to optimal
	// Set usage to depth stencil attachment bit
	// Set properties to device local
	pImageManager->CreateImage(pGPUObject, swapchainExtent.width, swapchainExtent.height, 1, m_MsaaSamples, depthFormat,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		m_DepthImage);

	// Create image view for the depth image
	m_DepthImage.imageView = pImageManager->CreateImageView(pGPUObject->GetDevice(), m_DepthImage.image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

	// Transition the depth image from undefined to depth stencil attachment optimal
	pImageManager->TransitionImageLayout(m_DepthImage.image, commandBuffer, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
}