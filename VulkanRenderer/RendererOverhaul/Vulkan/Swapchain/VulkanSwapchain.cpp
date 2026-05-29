// VulkanSwapchain.cpp

// Header include
#include "VulkanSwapchain.h"

// File includes
#include "Vulkan/Core/VulkanCore.h"
#include "Vulkan/Core/VulkanAllocator.h"

#include "Engine/Window/Window.h"
#include "Engine/ConfigManager.h"

// Standard library includes
#include <stdexcept>

DDM::VulkanSwapchain::VulkanSwapchain(const VulkanAllocator* pAllocator, const VulkanCore* pCore)
	:m_pAllocator(pAllocator),
	m_pCore(pCore)
{
	CreateSwapchain();
}

DDM::VulkanSwapchain::~VulkanSwapchain()
{
	vkDestroySwapchainKHR(m_pCore->GetDeviceHandle(), m_VkSwapchain, m_pAllocator->GetAllocator());
}

void DDM::VulkanSwapchain::CreateSwapchain()
{
	auto& window = Window::GetInstance();

	VkSwapchainCreateInfoKHR createInfo{};

	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.surface = VK_NULL_HANDLE;// Surface will be set up in VulkanCore object
	createInfo.minImageCount = static_cast<uint32_t>(ConfigManager::GetInstance().GetInt("SwapchainImageCount"));
	createInfo.imageFormat = VK_FORMAT_R8G8B8A8_SRGB;
	createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

	int width{};
	int height{};
	window.GetFrameBufferSize(width, height);
	createInfo.imageExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.queueFamilyIndexCount = 0;
	createInfo.pQueueFamilyIndices = nullptr;
	createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = m_VkSwapchain;

	m_pCore->ValidateSwapchainCreateInfo(createInfo);

	if (vkCreateSwapchainKHR(m_pCore->GetDeviceHandle(), &createInfo, m_pAllocator->GetAllocator(), &m_VkSwapchain) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create Vulkan swapchain!");
	}

	RetrieveImages();
}

void DDM::VulkanSwapchain::RetrieveImages()
{
	uint32_t imageCount{};

	if (vkGetSwapchainImagesKHR(m_pCore->GetDeviceHandle(), m_VkSwapchain, &imageCount, nullptr) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to get the number of swapchain images!");
	}

	std::vector<VkImage> images(imageCount);

	if (vkGetSwapchainImagesKHR(m_pCore->GetDeviceHandle(), m_VkSwapchain, &imageCount, images.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to get the swapchain images!");
	}

	m_SwapchainImages.reserve(imageCount);

	for (const auto& image : images)
	{
		m_SwapchainImages.push_back(VulkanSwapchainImage::Create(m_pAllocator, m_pCore, image));
	}
}
