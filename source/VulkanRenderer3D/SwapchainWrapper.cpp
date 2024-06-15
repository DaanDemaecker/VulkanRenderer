// SwapchainWrapper.cpp

// File includes
#include "SwapchainWrapper.h"
#include "GPUObject.h"
#include "ImageViewManager.h"
#include "Window.h"
#include "ImageManager.h"
#include "VulkanUtils.h"

// Standard library includes
#include <stdexcept>
#include <algorithm>

D3D::SwapchainWrapper::SwapchainWrapper(GPUObject* pGPUObject, VkSurfaceKHR surface,
	D3D::ImageManager* pImageManager, VkSampleCountFlagBits msaaSamples)
{
	// Create image view manager
	m_pImageViewManager = std::make_unique<ImageViewManager>(msaaSamples);

	// Initialize the swapchain
	CreateSwapChain(pGPUObject, surface);
	// Initialize the image views
	CreateSwapchainImageViews(pGPUObject->GetDevice(), pImageManager);
}

D3D::SwapchainWrapper::~SwapchainWrapper()
{
}

void D3D::SwapchainWrapper::SetupImageViews(GPUObject* pGPUObject, D3D::ImageManager* pImageManager,
	 VkCommandBuffer commandBuffer, VkRenderPass renderPass)
{
	// Creat the color image
	m_pImageViewManager->CreateColorResources(pGPUObject, m_SwapChainImageFormat, m_SwapChainExtent, pImageManager);
	// Create depth image
	m_pImageViewManager->CreateDepthResources(pGPUObject, m_SwapChainExtent, pImageManager, commandBuffer);
	// Initialize frame buffers
	CreateFramebuffers(pGPUObject->GetDevice(), renderPass);
}

void D3D::SwapchainWrapper::SetupSwapchain(GPUObject* pGPUObject, VkSurfaceKHR surface,
	D3D::ImageManager* pImageManager, VkCommandBuffer commandBuffer, VkRenderPass renderpass)
{
	// Initalize the swapchain
	CreateSwapChain(pGPUObject, surface);
	// Initialize swapchain image views
	CreateSwapchainImageViews(pGPUObject->GetDevice(), pImageManager);
	// Setup color and depth resources
	SetupImageViews(pGPUObject, pImageManager, commandBuffer, renderpass);
}

void D3D::SwapchainWrapper::CreateSwapChain(GPUObject* pGPUObject, VkSurfaceKHR surface)
{
	// Get device
	auto device{ pGPUObject->GetDevice() };

	// Get swapchain support details
	D3D::SwapChainSupportDetails swapChainSupport = VulkanUtils::QuerySwapChainSupport(pGPUObject->GetPhysicalDevice(), surface);
	// Get the format for the swapchain surface
	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	// Get presentmode for the swapchain
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
	// Get extent for swapchain
	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

	// Set minimum image count to minimum image count of swapchain capabilities
	m_MinImageCount = swapChainSupport.capabilities.minImageCount;
	// Create an image count and set it to the minimum image count of swapchain capabilities + 1
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

	// If maximum image count is greater than 0 and image count is greater than maximum image count
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		// Set image count to maximum image count of swapchain capabilities
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	// Creaate swapchain create info
	VkSwapchainCreateInfoKHR createInfo{};
	// Set type to swapchain create info
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	// Give surface handle
	createInfo.surface = surface;
	// Set minimum image count
	createInfo.minImageCount = imageCount;
	// Set format to the surface format
	createInfo.imageFormat = surfaceFormat.format;
	// Set color space to surface color space
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	// Set extent to chosen swapchain extent
	createInfo.imageExtent = extent;
	// Set array layers to 1
	createInfo.imageArrayLayers = 1;
	// Set image usage to color attachment
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	// Get the queuefamily indices
	D3D::QueueFamilyIndices indices = VulkanUtils::FindQueueFamilies(pGPUObject->GetPhysicalDevice(), surface);
	// Create array of graphics and present family indices
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	// If graphics and present family indices are not equal
	if (indices.graphicsFamily != indices.presentFamily)
	{
		//Image is owned by queue family and ownership must be transfered
		// Set sharing mode to concurrent
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		// Set index count to 2
		createInfo.queueFamilyIndexCount = 2;
		// Give array of indices
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		///Images can be used across multiple queue families without transfer of ownership
		// Set sharing mode to exclusove
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		// Set index count to 0
		createInfo.queueFamilyIndexCount = 0;
		// Give nullptr for indices
		createInfo.pQueueFamilyIndices = nullptr;
	}

	// Set pre transform to current transform of swapchain capabilities
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	// Set composite alpha to opague
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	// Set presentmode to chosen presentmode
	createInfo.presentMode = presentMode;
	// Set clipped to 2
	createInfo.clipped = VK_TRUE;
	// Set old swapchain to null handle
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	// Create the swapchain
	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to create swap chain!");
	}

	// Get the swapchain image count
	vkGetSwapchainImagesKHR(device, m_SwapChain, &imageCount, nullptr);
	// Resize swapchain images to size of image count
	m_SwapChainImages.resize(imageCount);
	// Get swapchain images 
	vkGetSwapchainImagesKHR(device, m_SwapChain, &imageCount, m_SwapChainImages.data());

	// Save swapchain format
	m_SwapChainImageFormat = surfaceFormat.format;
	// Save swapchain extent
	m_SwapChainExtent = extent;
}

void D3D::SwapchainWrapper::CreateSwapchainImageViews(VkDevice device, D3D::ImageManager* pImageManager)
{
	// Resize image views to the size of images
	m_SwapChainImageViews.resize(m_SwapChainImages.size());

	// Loop trough the amount of images
	for (size_t i = 0; i < m_SwapChainImages.size(); i++)
	{
		// Create image view
		m_SwapChainImageViews[i] = pImageManager->CreateImageView(device, m_SwapChainImages[i], m_SwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
}

void D3D::SwapchainWrapper::Cleanup(VkDevice device)
{
	// Loop trough the amount of swapchain framebuffers
	for (size_t i = 0; i < m_SwapChainFramebuffers.size(); ++i)
	{
		// Delete the framebuffer
		vkDestroyFramebuffer(device, m_SwapChainFramebuffers[i], nullptr);
	}

	// Loop trough the amount of image views
	for (size_t i = 0; i < m_SwapChainImageViews.size(); ++i)
	{
		// Destroy the image view
		vkDestroyImageView(device, m_SwapChainImageViews[i], nullptr);
	}

	// Destroy the swapchain
	vkDestroySwapchainKHR(device, m_SwapChain, nullptr);

	// Clean up image view manager
	m_pImageViewManager->Cleanup(device);
}

void D3D::SwapchainWrapper::RecreateSwapChain(GPUObject* pGPUObject, VkSurfaceKHR surface,
	D3D::ImageManager* pImageManager, VkCommandBuffer commandBuffer, VkRenderPass renderpass)
{
	// Call cleanup function to destroy all allocated objects
	Cleanup(pGPUObject->GetDevice());
	
	// Set swapchain again
	SetupSwapchain(pGPUObject, surface, pImageManager, commandBuffer, renderpass);
}

VkSampleCountFlagBits D3D::SwapchainWrapper::GetMsaaSamples() const
{
	return m_pImageViewManager->GetMsaaSamples();
}

VkImageView D3D::SwapchainWrapper::GetDepthImage() const
{
	return m_pImageViewManager->GetDepthImageView();
}


VkExtent2D D3D::SwapchainWrapper::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	// If the width of the current extent isn't equal to the maximum uint32_t, return the current extent
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	// If not create new values
	else
	{
		// Create width and height ints
		int width, height;
		// Get the width and height of the window
		glfwGetFramebufferSize(Window::GetInstance().GetWindowStruct().pWindow, &width, &height);
		// Create extent that is the size of the window
		VkExtent2D actualExtent =
		{
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		// Clamp width between minimum and maximum width capabilities
		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		// Clamlp height between minimum and maximum height capabilities
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		// Return the newly created extent object
		return actualExtent;
	}
}

void D3D::SwapchainWrapper::CreateFramebuffers(VkDevice device, VkRenderPass renderpass)
{
	// Resize framebuffers to size of imageviews
	m_SwapChainFramebuffers.resize(m_SwapChainImageViews.size());

	// Loop trough the amount of imageViews
	for (size_t i = 0; i < m_SwapChainImageViews.size(); ++i)
	{
		// Create array for image views
		std::array<VkImageView, 3> attachments =
		{
			m_pImageViewManager->GetColorImageView(),
			m_pImageViewManager->GetDepthImageView(),
			m_SwapChainImageViews[i]
		};

		// Create framebuffer create info
		VkFramebufferCreateInfo framebufferInfo{};
		// Set type to framebuffer create info
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		// Give handle of renderpass
		framebufferInfo.renderPass = renderpass;
		// Set attachment count to size of attachments
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		// Give pointer to data of attachments
		framebufferInfo.pAttachments = attachments.data();
		// Set width to swapchainextent width
		framebufferInfo.width = m_SwapChainExtent.width;
		// Set height to swapchain extent height
		framebufferInfo.height = m_SwapChainExtent.height;
		// Set amount of layers to 1
		framebufferInfo.layers = 1;

		// Create framebuffers
		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_SwapChainFramebuffers[i]) != VK_SUCCESS)
		{
			// If unsuccessful, throw runtime error
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

VkSurfaceFormatKHR D3D::SwapchainWrapper::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	// Loop trough the available formats
	for (const auto& availableFormat : availableFormats)
	{
		// I format is equal to B8G8R8A8 and colorspace is srgb nonlinear, return format
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}

	// If no suitable format was found, return first in the vector of formats
	return availableFormats[0];
}

VkPresentModeKHR D3D::SwapchainWrapper::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	// Loop trough the available presentModes
	for (const auto& availablePresentMode : availablePresentModes)
	{
		// If current presentmode is mailbox, return it
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
	}

	// If no suitable format was found, return fifo mode
	return VK_PRESENT_MODE_FIFO_KHR;
}