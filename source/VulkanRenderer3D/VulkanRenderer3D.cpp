#include "VulkanRenderer3D.h"
#include "D3DEngine.h"
#include "Utils.h"
#include "Model.h"
#include "DescriptorPoolManager.h"
#include "DescriptorPoolWrapper.h"

#include "ImGuiIncludes.h"
#include "ImGuiWrapper.h"
#include "PipelineManager.h"
#include "InstanceWrapper.h"
#include "ImageManager.h"
#include "CommandpoolManager.h"
#include "RenderpassWrapper.h"


#include <set>
#include <algorithm>
#include <iostream>

extern D3D::Window g_pWindow;

D3D::VulkanRenderer3D::VulkanRenderer3D()
{
#ifdef NDEBUG
	m_EnableValidationLayers = false;
#endif

	m_GlobalLight.direction = glm::normalize(glm::vec3{ -.577, -.577f, .577 });
	m_GlobalLight.color = glm::vec3{ 1.f, 1.f, 1.f };
	m_GlobalLight.intensity = 1.f;

	InitVulkan();

	InitImGui();
}

D3D::VulkanRenderer3D::~VulkanRenderer3D()
{
	vkDeviceWaitIdle(m_Device);

	CleanupImGui();

	CleanupVulkan();
}

void D3D::VulkanRenderer3D::CleanupVulkan()
{
	CleanupSwapChain();

	m_pImageManager->Cleanup(m_Device);

	for(size_t i{}; i < m_MaxFramesInFlight; i++)
	{
		vkDestroyBuffer(m_Device, m_LightBuffers[i], nullptr);
		vkFreeMemory(m_Device, m_LightMemory[i], nullptr);
	}

	m_pDescriptorPoolManager->Cleanup(m_Device);

	m_pPipelineManager->Cleanup(m_Device);

	m_pRenderpassWrapper->cleanup(m_Device);

	for (size_t i = 0; i < m_MaxFramesInFlight; ++i)
	{
		vkDestroySemaphore(m_Device, m_ImageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(m_Device, m_RenderFinishedSemaphores[i], nullptr);
		vkDestroyFence(m_Device, m_InFlightFences[i], nullptr);
	}

	m_pCommandPoolManager->Cleanup(m_Device);

	vkDestroyDevice(m_Device, nullptr);

	vkDestroySurfaceKHR(m_pInstanceWrapper->GetInstance(), m_Surface, nullptr);

	m_pInstanceWrapper->cleanup(m_EnableValidationLayers);
}

void D3D::VulkanRenderer3D::CleanupImGui()
{
	m_pImGuiWrapper->Cleanup(m_Device);
}

void D3D::VulkanRenderer3D::InitVulkan()
{
	m_pInstanceWrapper = std::make_unique<InstanceWrapper>(m_EnableValidationLayers, m_ValidationLayers);

	CreateSurface();
	PickPhysicalDevice();
	CreateLogicalDevice();

	m_pCommandPoolManager = std::make_unique<CommandpoolManager>(m_Device, m_PhysicalDevice, m_Surface, static_cast<uint32_t>(m_MaxFramesInFlight));

	m_pImageManager = std::make_unique<ImageManager>(m_Device, m_PhysicalDevice, m_pCommandPoolManager.get(), m_GraphicsQueue);

	CreateSwapChain();
	CreateImageViews();

	m_pRenderpassWrapper = std::make_unique<RenderpassWrapper>(m_Device, m_SwapChainImageFormat, FindDepthFormat(), m_MsaaSamples);

	CreateLightBuffer();

	
	CreateColorResources();
	CreateDepthResources();
	CreateFramebuffers();

	m_pDescriptorPoolManager = std::make_unique<DescriptorPoolManager>();

	m_pPipelineManager = std::make_unique<PipelineManager>(m_DefaultPipelineName, m_DefaultVertName, m_DefaultFragName);

	AddGraphicsPipeline(m_DefaultPipelineName, m_DefaultVertName, m_DefaultFragName, 1, 1, 0);

	CreateSyncObjects();

}

void D3D::VulkanRenderer3D::InitImGui()
{
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = m_pInstanceWrapper->GetInstance(); // Your Vulkan instance
	init_info.PhysicalDevice = m_PhysicalDevice; // Your Vulkan physical device
	init_info.Device = m_Device; // Your Vulkan logical device
	init_info.QueueFamily = m_GraphicsQueueIndex; // The index of your Vulkan queue family that supports graphics operations
	init_info.Queue = m_GraphicsQueue; // Your Vulkan graphics queue
	init_info.PipelineCache = VK_NULL_HANDLE;
	init_info.Allocator = VK_NULL_HANDLE;
	init_info.MinImageCount = m_MinImageCount; // Minimum number of swapchain images
	init_info.ImageCount = static_cast<uint32_t>(m_MaxFramesInFlight); // Number of swapchain images
	init_info.CheckVkResultFn = [](VkResult /*err*/) { /* Implement your own error handling */ };
	init_info.MSAASamples = m_MsaaSamples;

	auto commandBuffer{ BeginSingleTimeCommands() };

	m_pImGuiWrapper = std::make_unique<D3D::ImGuiWrapper>(init_info, m_pRenderpassWrapper->GetRenderpass(), commandBuffer, m_Device, static_cast<uint32_t>(m_MaxFramesInFlight));

	EndSingleTimeCommands(commandBuffer);
}

void D3D::VulkanRenderer3D::AddGraphicsPipeline(const std::string& pipelineName, const std::string& vertShaderName, const std::string& fragShaderName, int vertexUbos, int fragmentUbos, int textureAmount)
{
	m_pPipelineManager->AddGraphicsPipeline(m_Device, static_cast<uint32_t>(m_MaxFramesInFlight), m_pRenderpassWrapper->GetRenderpass(), m_MsaaSamples,
		pipelineName, vertShaderName, fragShaderName, vertexUbos, fragmentUbos, textureAmount);
}

void D3D::VulkanRenderer3D::Render(std::vector<std::unique_ptr<Model>>& pModels)
{
	vkWaitForFences(m_Device, 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex{};
	VkResult result = vkAcquireNextImageKHR(m_Device, m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		RecreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	vkResetFences(m_Device, 1, &m_InFlightFences[m_CurrentFrame]);

	auto commandBuffer{m_pCommandPoolManager->GetCommandBuffer(m_CurrentFrame)};

	vkResetCommandBuffer(commandBuffer, 0);

	RecordCommandBuffer(commandBuffer, imageIndex, pModels);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrame]};
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[m_CurrentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_InFlightFences[m_CurrentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { m_SwapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	presentInfo.pResults = nullptr;

	result = vkQueuePresentKHR(m_PresentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || g_pWindow.FrameBufferResized)
	{
		RecreateSwapChain();
		g_pWindow.FrameBufferResized = false;
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}

	++m_CurrentFrame %= m_MaxFramesInFlight;
}

void D3D::VulkanRenderer3D::RecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex, std::vector<std::unique_ptr<Model>>& pModels)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_pRenderpassWrapper->GetRenderpass();
	renderPassInfo.framebuffer = m_SwapChainFramebuffers[imageIndex];

	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = m_SwapChainExtent;

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { {0.388f, 0.588f, 0.929f, 1.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_SwapChainExtent.width);
	viewport.height = static_cast<float>(m_SwapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = m_SwapChainExtent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	UpdateLightBuffer(m_CurrentFrame);

	for (size_t i = 0; i < pModels.size(); ++i)
	{
		pModels[i]->Render();
	}

	m_pImGuiWrapper->Render(commandBuffer);

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}
}

void D3D::VulkanRenderer3D::Render(Model* pModel, VkCommandBuffer& commandBuffer, const VkDescriptorSet* descriptorSet, const PipelinePair& pipeline)
{
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline);

	//vkCmdPushConstants(commandBuffer, pipeline.pipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(LightObject), &m_GlobalLight);

	VkBuffer vertexBuffers[] = { pModel->GetVertexBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(commandBuffer, pModel->GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout, 0, 1, descriptorSet, 0, nullptr);

	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(pModel->GetIndexAmount()), 1, 0, 0, 0);
}

VkImageView& D3D::VulkanRenderer3D::GetDefaultImageView()
{
	return m_pImageManager->GetDefaultImageView();
}

VkSampler& D3D::VulkanRenderer3D::GetSampler()
{
	return m_pImageManager->GetSampler();
}

D3D::PipelinePair& D3D::VulkanRenderer3D::GetPipeline(const std::string& name)
{
	return m_pPipelineManager->GetPipeline(name);
}

VkCommandBuffer& D3D::VulkanRenderer3D::GetCurrentCommandBuffer()
{
	return m_pCommandPoolManager->GetCommandBuffer(m_CurrentFrame);
}

D3D::DescriptorPoolManager* D3D::VulkanRenderer3D::GetDescriptorPoolManager() const
{
	return m_pDescriptorPoolManager.get();
}

void D3D::VulkanRenderer3D::CreateSurface()
{
	if (glfwCreateWindowSurface(m_pInstanceWrapper->GetInstance(), g_pWindow.pWindow, nullptr, &m_Surface) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create window surface!");
	}
}

void D3D::VulkanRenderer3D::PickPhysicalDevice()
{
	//Get number of physical devices that support Vulkan
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_pInstanceWrapper->GetInstance(), &deviceCount, nullptr);

	//If no physical devices found, throw runtime error
	if (deviceCount == 0)
	{
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	//Allocate array to hold physical devices handles
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_pInstanceWrapper->GetInstance(), &deviceCount, devices.data());

	//use an ordered map to automatically sort candidates by increasing score
	std::multimap<int, VkPhysicalDevice> candidates;

	for (const auto& device : devices)
	{
		if (IsDeviceSuitable(device))
		{
			m_PhysicalDevice = device;
			m_MsaaSamples = GetMaxUsableSampleCount();
			break;
		}
	}

	if (m_PhysicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

bool D3D::VulkanRenderer3D::IsDeviceSuitable(VkPhysicalDevice device)
{
	QueueFamilyIndices indices = FindQueueFamilies(device);

	bool extensionsSupported = CheckDeviceExtensionSupport(device);

	bool swapChainAdequate = false;

	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	VkPhysicalDeviceFeatures supportedFeatures{};
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

D3D::QueueFamilyIndices D3D::VulkanRenderer3D::FindQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	//find at least one queue family that supports VK_QUEUE_GRAPHICS_BIT
	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);

		if (presentSupport)
		{
			indices.presentFamily = i;
		}

		if (indices.isComplete())
			break;

		i++;
	}

	return indices;
}

bool D3D::VulkanRenderer3D::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
	//Check how many extensions this device supports
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	//Get a list of all available extensions
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	//Create a set of required extensions to avoid duplicates
	std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

	//Clear all available extensions from the required ones
	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	//If the required extensions are empty, they are all available
	return requiredExtensions.empty();

	return false;
}

D3D::SwapChainSupportDetails D3D::VulkanRenderer3D::QuerySwapChainSupport(VkPhysicalDevice device)
{
	D3D::SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

void D3D::VulkanRenderer3D::CreateLogicalDevice()
{
	QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	m_GraphicsQueueIndex = indices.graphicsFamily.value();

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	deviceFeatures.sampleRateShading = VK_TRUE;

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();


	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
	createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

	if (m_EnableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
		createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(m_Device, indices.graphicsFamily.value(), 0, &m_GraphicsQueue);
	vkGetDeviceQueue(m_Device, indices.presentFamily.value(), 0, &m_PresentQueue);
}


void D3D::VulkanRenderer3D::CreateSwapChain()
{
	D3D::SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(m_PhysicalDevice);

	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	m_MinImageCount = swapChainSupport.capabilities.minImageCount;

	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_Surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	

	QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice);
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

	if (vkCreateSwapchainKHR(m_Device, &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, nullptr);
	m_SwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, m_SwapChainImages.data());

	m_SwapChainImageFormat = surfaceFormat.format;
	m_SwapChainExtent = extent;
}

VkSurfaceFormatKHR D3D::VulkanRenderer3D::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR D3D::VulkanRenderer3D::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D D3D::VulkanRenderer3D::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
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

void D3D::VulkanRenderer3D::CleanupSwapChain()
{
	for (size_t i = 0; i < m_SwapChainFramebuffers.size(); ++i)
	{
		vkDestroyFramebuffer(m_Device, m_SwapChainFramebuffers[i], nullptr);
	}

	for (size_t i = 0; i < m_SwapChainImageViews.size(); ++i)
	{
		vkDestroyImageView(m_Device, m_SwapChainImageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);

	m_DepthImage.cleanup(m_Device);
	m_ColorImage.cleanup(m_Device);
}

void D3D::VulkanRenderer3D::RecreateSwapChain()
{
	g_pWindow.Width = 0;
	g_pWindow.Height = 0;
	glfwGetFramebufferSize(g_pWindow.pWindow, &g_pWindow.Width, &g_pWindow.Height);
	while (g_pWindow.Width == 0 || g_pWindow.Height == 0)
	{
		glfwGetFramebufferSize(g_pWindow.pWindow, &g_pWindow.Width, &g_pWindow.Height);
		glfwWaitEvents();
	}


	vkDeviceWaitIdle(m_Device);

	CleanupSwapChain();

	CreateSwapChain();
	CreateImageViews();
	CreateColorResources();
	CreateDepthResources();
	CreateFramebuffers();
}

void D3D::VulkanRenderer3D::CreateImageViews()
{
	m_SwapChainImageViews.resize(m_SwapChainImages.size());

	for (size_t i = 0; i < m_SwapChainImages.size(); i++)
	{
		m_SwapChainImageViews[i] = m_pImageManager->CreateImageView(m_Device, m_SwapChainImages[i], m_SwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
}

void D3D::VulkanRenderer3D::CreateColorResources()
{
	VkFormat colorFormat = m_SwapChainImageFormat;

	m_pImageManager->CreateImage(m_Device, m_PhysicalDevice, m_SwapChainExtent.width, m_SwapChainExtent.height, 1, m_MsaaSamples, colorFormat,
		VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_ColorImage);

	m_ColorImage.imageView = m_pImageManager->CreateImageView(m_Device, m_ColorImage.image, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

VkFormat D3D::VulkanRenderer3D::FindDepthFormat()
{
	return FindSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

void D3D::VulkanRenderer3D::CreateFramebuffers()
{
	m_SwapChainFramebuffers.resize(m_SwapChainImageViews.size());

	for (size_t i = 0; i < m_SwapChainImageViews.size(); ++i)
	{
		std::array<VkImageView, 3> attachments =
		{
			m_ColorImage.imageView,
			m_DepthImage.imageView,
			m_SwapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_pRenderpassWrapper->GetRenderpass();
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = m_SwapChainExtent.width;
		framebufferInfo.height = m_SwapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(m_Device, &framebufferInfo, nullptr, &m_SwapChainFramebuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void D3D::VulkanRenderer3D::CreateSyncObjects()
{
	m_ImageAvailableSemaphores.resize(m_MaxFramesInFlight);
	m_RenderFinishedSemaphores.resize(m_MaxFramesInFlight);
	m_InFlightFences.resize(m_MaxFramesInFlight);


	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;


	for (size_t i = 0; i < m_MaxFramesInFlight; ++i)
	{
		if (vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(m_Device, &fenceInfo, nullptr, &m_InFlightFences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create sync objects!");
		}
	}
}

void D3D::VulkanRenderer3D::CreateLightBuffer()
{
	VkDeviceSize bufferSize = sizeof(DirectionalLightObject);

	m_LightBuffers.resize(m_MaxFramesInFlight);
	m_LightMemory.resize(m_MaxFramesInFlight);
	m_LightMapped.resize(m_MaxFramesInFlight);

	m_LightChanged.resize(m_MaxFramesInFlight);
	std::fill(m_LightChanged.begin(), m_LightChanged.end(), true);



	for (size_t i = 0; i < m_MaxFramesInFlight; ++i)
	{
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			m_LightBuffers[i], m_LightMemory[i]);

		vkMapMemory(m_Device, m_LightMemory[i], 0, bufferSize, 0, &m_LightMapped[i]);

		UpdateLightBuffer(static_cast<int>(i));
	}
}

void D3D::VulkanRenderer3D::UpdateLightBuffer(int frame)
{
	//if (!m_LightChanged[frame])
	//	return;

	m_LightChanged[frame] = false;

	memcpy(m_LightMapped[frame], &m_GlobalLight, sizeof(m_GlobalLight));
}

VkSampleCountFlagBits D3D::VulkanRenderer3D::GetMaxUsableSampleCount()
{

	VkPhysicalDeviceProperties physicalDeviceProperties{};
	vkGetPhysicalDeviceProperties(m_PhysicalDevice, &physicalDeviceProperties);

	VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts &
		physicalDeviceProperties.limits.framebufferDepthSampleCounts;

	if (counts & VK_SAMPLE_COUNT_64_BIT)
		return VK_SAMPLE_COUNT_64_BIT;
	if (counts & VK_SAMPLE_COUNT_32_BIT)
		return VK_SAMPLE_COUNT_32_BIT;
	if (counts & VK_SAMPLE_COUNT_16_BIT)
		return VK_SAMPLE_COUNT_16_BIT;
	if (counts & VK_SAMPLE_COUNT_8_BIT)
		return VK_SAMPLE_COUNT_8_BIT;
	if (counts & VK_SAMPLE_COUNT_4_BIT)
		return VK_SAMPLE_COUNT_4_BIT;
	if (counts & VK_SAMPLE_COUNT_2_BIT)
		return VK_SAMPLE_COUNT_2_BIT;

	return VK_SAMPLE_COUNT_1_BIT;
}

void D3D::VulkanRenderer3D::CreateDepthResources()
{
	VkFormat depthFormat = FindDepthFormat();

	m_pImageManager->CreateImage(m_Device, m_PhysicalDevice, m_SwapChainExtent.width, m_SwapChainExtent.height, 1, m_MsaaSamples, depthFormat,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		m_DepthImage);

	m_DepthImage.imageView = m_pImageManager->CreateImageView(m_Device, m_DepthImage.image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

	TransitionImageLayout(m_DepthImage.image, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
}

VkFormat D3D::VulkanRenderer3D::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for (const VkFormat& format : candidates)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
		{
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
		{
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");
}

uint32_t D3D::VulkanRenderer3D::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
	{
		if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

void D3D::VulkanRenderer3D::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)
{
	VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

	m_pImageManager->TransitionImageLayout(image, commandBuffer, format, oldLayout, newLayout, mipLevels);

	EndSingleTimeCommands(commandBuffer);
}

void D3D::VulkanRenderer3D::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(m_Device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(m_Device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(m_Device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(m_Device, buffer, bufferMemory, 0);
}

void D3D::VulkanRenderer3D::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

	VkBufferCopy copyRegion{};
	copyRegion.size = size;

	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	EndSingleTimeCommands(commandBuffer);
}

void D3D::VulkanRenderer3D::UpdateUniformBuffer(UniformBufferObject& buffer)
{
	glm::vec3 cameraPosition(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraScale(1.0f, 1.0f, 1.0f);
	glm::vec3 cameraRotation(0.0f, 0.0f, 0.0f);

	buffer.view = glm::mat4{ 1.0f }; // Identity matrix

	// Apply translation
	buffer.view = glm::translate(buffer.view, cameraPosition);

	// Apply scaling
	buffer.view = glm::scale(buffer.view, cameraScale);

	// Apply rotation
	buffer.view = glm::rotate(buffer.view, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));


	buffer.proj = glm::perspective(glm::radians(45.0f), m_SwapChainExtent.width / static_cast<float>(m_SwapChainExtent.height), 0.1f, 100.0f);
	buffer.proj[1][1] *= -1;
}

std::vector<VkDescriptorSetLayout>& D3D::VulkanRenderer3D::GetDescriptorSetLayout(int vertexUbos, int fragmentUbos, int textureAmount)
{
	return m_pPipelineManager->GetDescriptorSetLayout(m_Device, static_cast<uint32_t>(m_MaxFramesInFlight), vertexUbos, fragmentUbos, textureAmount);
}

void D3D::VulkanRenderer3D::CreateTexture(Texture& texture, const std::string& textureName, uint32_t& mipLevels)
{
	m_pImageManager->CreateTextureImage(m_Device, m_PhysicalDevice, texture, textureName, mipLevels, m_pCommandPoolManager.get(), m_GraphicsQueue);
	texture.imageView = m_pImageManager->CreateImageView(m_Device, texture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);
}

VkCommandBuffer D3D::VulkanRenderer3D::BeginSingleTimeCommands()
{
	return m_pCommandPoolManager->BeginSingleTimeCommands(m_Device);
}

void D3D::VulkanRenderer3D::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
{
	m_pCommandPoolManager->EndSingleTimeCommands(m_Device, commandBuffer, m_GraphicsQueue);
}
