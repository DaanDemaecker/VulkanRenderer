// SwapchainWrapper.cpp

// File includes
#include "SwapchainWrapper.h"
#include "D3DEngine.h"
#include "ImageManager.h"
#include "VulkanUtils.h"

// Standard library includes
#include <stdexcept>
#include <algorithm>

extern D3D::Window g_pWindow;

D3D::SwapchainWrapper::SwapchainWrapper(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
	D3D::ImageManager* pImageManager, VkSampleCountFlagBits msaaSamples)
{
	m_pImageViewManager = std::make_unique<ImageViewManager>(msaaSamples);

	CreateSwapChain(device, physicalDevice, surface);
	CreateImageViews(device, pImageManager);
}

void D3D::SwapchainWrapper::SetupImageViews(VkDevice device, VkPhysicalDevice physicalDevice, D3D::ImageManager* pImageManager,
	VkFormat depthFormat, VkCommandBuffer commandBuffer, VkRenderPass renderPass)
{
	m_pImageViewManager->CreateColorResources(device, physicalDevice, m_SwapChainImageFormat, m_SwapChainExtent, pImageManager);
	m_pImageViewManager->CreateDepthResources(device, physicalDevice, depthFormat, m_SwapChainExtent, pImageManager, commandBuffer);
	CreateFramebuffers(device, renderPass);
}

void D3D::SwapchainWrapper::SetupSwapchain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
	D3D::ImageManager* pImageManager, VkCommandBuffer commandBuffer, VkFormat depthFormat, VkRenderPass renderpass)
{
	CreateSwapChain(device, physicalDevice, surface);
	CreateImageViews(device, pImageManager);
	m_pImageViewManager->CreateColorResources(device, physicalDevice, m_SwapChainImageFormat, m_SwapChainExtent, pImageManager);
	m_pImageViewManager->CreateDepthResources(device, physicalDevice, depthFormat, m_SwapChainExtent, pImageManager, commandBuffer);
	CreateFramebuffers(device, renderpass);
}

void D3D::SwapchainWrapper::CreateSwapChain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
	D3D::SwapChainSupportDetails swapChainSupport = VulkanUtils::QuerySwapChainSupport(physicalDevice, surface);

	VkSurfaceFormatKHR surfaceFormat = VulkanUtils::ChooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = VulkanUtils::ChooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	m_MinImageCount = swapChainSupport.capabilities.minImageCount;

	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;


	D3D::QueueFamilyIndices indices = VulkanUtils::FindQueueFamilies(physicalDevice, surface);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily)
	{
		//Image is owned by queue family and ownership must be transfered
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		///Images can be used across multiple queue families without transfer of ownership
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; //Optional
		createInfo.pQueueFamilyIndices = nullptr; //Optional
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(device, m_SwapChain, &imageCount, nullptr);
	m_SwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, m_SwapChain, &imageCount, m_SwapChainImages.data());

	m_SwapChainImageFormat = surfaceFormat.format;
	m_SwapChainExtent = extent;
}

void D3D::SwapchainWrapper::CreateImageViews(VkDevice device, D3D::ImageManager* pImageManager)
{
	m_SwapChainImageViews.resize(m_SwapChainImages.size());

	for (size_t i = 0; i < m_SwapChainImages.size(); i++)
	{
		m_SwapChainImageViews[i] = pImageManager->CreateImageView(device, m_SwapChainImages[i], m_SwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
}

void D3D::SwapchainWrapper::Cleanup(VkDevice device)
{
	for (size_t i = 0; i < m_SwapChainFramebuffers.size(); ++i)
	{
		vkDestroyFramebuffer(device, m_SwapChainFramebuffers[i], nullptr);
	}

	for (size_t i = 0; i < m_SwapChainImageViews.size(); ++i)
	{
		vkDestroyImageView(device, m_SwapChainImageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(device, m_SwapChain, nullptr);

	m_pImageViewManager->Cleanup(device);
}

void D3D::SwapchainWrapper::RecreateSwapChain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
	D3D::ImageManager* pImageManager, VkCommandBuffer commandBuffer, VkFormat depthFormat, VkRenderPass renderpass)
{
	Cleanup(device);
	
	SetupSwapchain(device, physicalDevice, surface, pImageManager, commandBuffer, depthFormat, renderpass);
}


VkExtent2D D3D::SwapchainWrapper::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		int width, height;
		glfwGetFramebufferSize(g_pWindow.pWindow, &width, &height);
		VkExtent2D actualExtent =
		{
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

void D3D::SwapchainWrapper::CreateFramebuffers(VkDevice device, VkRenderPass renderpass)
{
	m_SwapChainFramebuffers.resize(m_SwapChainImageViews.size());

	for (size_t i = 0; i < m_SwapChainImageViews.size(); ++i)
	{
		std::array<VkImageView, 3> attachments =
		{
			m_pImageViewManager->GetColorImageView(),
			m_pImageViewManager->GetDepthImageView(),
			m_SwapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderpass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = m_SwapChainExtent.width;
		framebufferInfo.height = m_SwapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_SwapChainFramebuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}