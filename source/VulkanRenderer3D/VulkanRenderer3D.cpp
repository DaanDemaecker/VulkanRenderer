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
#include "SwapchainWrapper.h"
#include "SyncObjectManager.h"

#include "DirectionalLightObject.h"

#include <set>
#include <algorithm>
#include <iostream>

extern D3D::Window g_pWindow;

D3D::VulkanRenderer3D::VulkanRenderer3D()
{
#ifdef NDEBUG
	m_EnableValidationLayers = false;
#endif

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
	m_pSwapchainWrapper->Cleanup(m_Device);

	m_pImageManager->Cleanup(m_Device);

	m_pGlobalLight->Cleanup(m_Device);

	m_pDescriptorPoolManager->Cleanup(m_Device);

	m_pPipelineManager->Cleanup(m_Device);

	m_pRenderpassWrapper->cleanup(m_Device);

	m_pSyncObjectManager->Cleanup(m_Device);

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

	m_pCommandPoolManager = std::make_unique<CommandpoolManager>(m_Device, m_PhysicalDevice, m_Surface, m_MaxFramesInFlight);

	m_pImageManager = std::make_unique<ImageManager>(m_Device, m_PhysicalDevice,
		m_pCommandPoolManager.get(), m_GraphicsQueue);

	auto msaaSamples = GetMaxUsableSampleCount();

	m_pSwapchainWrapper = std::make_unique<SwapchainWrapper>(m_Device, m_PhysicalDevice, m_Surface,
		m_pImageManager.get(), msaaSamples);

	m_pRenderpassWrapper = std::make_unique<RenderpassWrapper>(m_Device, m_pSwapchainWrapper->GetFormat(), FindDepthFormat(), msaaSamples);

	auto commandBuffer{ m_pCommandPoolManager->BeginSingleTimeCommands(m_Device) };
	m_pSwapchainWrapper->SetupImageViews(m_Device, m_PhysicalDevice, m_pImageManager.get(), FindDepthFormat(),
		commandBuffer, m_pRenderpassWrapper->GetRenderpass());
	m_pCommandPoolManager->EndSingleTimeCommands(m_Device, commandBuffer, m_GraphicsQueue);


	m_pGlobalLight = std::make_unique<DirectionalLightObject>(this);
	m_pGlobalLight->SetDirection(glm::vec3{-.577, -.577f, .577});
	m_pGlobalLight->SetColor(glm::vec3{1.f, 1.f, 1.f} );
	m_pGlobalLight->SetIntensity(1.f);


	m_pDescriptorPoolManager = std::make_unique<DescriptorPoolManager>();

	m_pPipelineManager = std::make_unique<PipelineManager>(m_DefaultPipelineName, m_DefaultVertName, m_DefaultFragName);

	AddGraphicsPipeline(m_DefaultPipelineName, m_DefaultVertName, m_DefaultFragName, 1, 1, 0);

	m_pSyncObjectManager = std::make_unique<SyncObjectManager>(m_Device, m_MaxFramesInFlight);
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
	init_info.MinImageCount = m_pSwapchainWrapper->GetMinImageCount(); // Minimum number of swapchain images
	init_info.ImageCount = m_MaxFramesInFlight; // Number of swapchain images
	init_info.CheckVkResultFn = [](VkResult /*err*/) { /* Implement your own error handling */ };
	init_info.MSAASamples = m_pSwapchainWrapper->GetMsaaSamples();

	auto commandBuffer{ BeginSingleTimeCommands() };

	m_pImGuiWrapper = std::make_unique<D3D::ImGuiWrapper>(init_info, m_pRenderpassWrapper->GetRenderpass(), commandBuffer, m_Device, m_MaxFramesInFlight);

	EndSingleTimeCommands(commandBuffer);
}

void D3D::VulkanRenderer3D::AddGraphicsPipeline(const std::string& pipelineName, const std::string& vertShaderName, const std::string& fragShaderName, int vertexUbos, int fragmentUbos, int textureAmount)
{
	m_pPipelineManager->AddGraphicsPipeline(m_Device, m_MaxFramesInFlight, m_pRenderpassWrapper->GetRenderpass(),
		m_pSwapchainWrapper->GetMsaaSamples(), pipelineName, vertShaderName, fragShaderName,
		vertexUbos, fragmentUbos, textureAmount);
}

void D3D::VulkanRenderer3D::Render(std::vector<std::unique_ptr<Model>>& pModels)
{
	vkWaitForFences(m_Device, 1, &m_pSyncObjectManager->GetInFlightFence(m_CurrentFrame), VK_TRUE, UINT64_MAX);

	uint32_t imageIndex{};
	VkResult result = vkAcquireNextImageKHR(m_Device, m_pSwapchainWrapper->GetSwapchain(), UINT64_MAX, m_pSyncObjectManager->GetImageAvailableSemaphore(m_CurrentFrame), VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		RecreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	vkResetFences(m_Device, 1, &m_pSyncObjectManager->GetInFlightFence(m_CurrentFrame));

	auto commandBuffer{m_pCommandPoolManager->GetCommandBuffer(m_CurrentFrame)};

	vkResetCommandBuffer(commandBuffer, 0);

	RecordCommandBuffer(commandBuffer, imageIndex, pModels);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { m_pSyncObjectManager->GetImageAvailableSemaphore(m_CurrentFrame)};
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	VkSemaphore signalSemaphores[] = { m_pSyncObjectManager->GetRenderFinishedSemaphore(m_CurrentFrame) };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_pSyncObjectManager->GetInFlightFence(m_CurrentFrame)) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { m_pSwapchainWrapper->GetSwapchain()};
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
	auto swapchainExtent{ m_pSwapchainWrapper->GetExtent() };

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
	renderPassInfo.framebuffer = m_pSwapchainWrapper->GetFrameBuffer(imageIndex);

	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapchainExtent;

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { {0.388f, 0.588f, 0.929f, 1.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapchainExtent.width);
	viewport.height = static_cast<float>(swapchainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapchainExtent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	m_pGlobalLight->UpdateBuffer(m_CurrentFrame);

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

const D3D::DirectionalLightStruct& D3D::VulkanRenderer3D::GetGlobalLight() const
{
	return m_pGlobalLight->GetLight();
}

std::vector<VkBuffer>& D3D::VulkanRenderer3D::GetLightBuffers()
{
	return m_pGlobalLight->GetLightBuffers();
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
	
	auto commandBuffer{ m_pCommandPoolManager->BeginSingleTimeCommands(m_Device) };

	m_pSwapchainWrapper->RecreateSwapChain(m_Device, m_PhysicalDevice, m_Surface, m_pImageManager.get(), commandBuffer,
		FindDepthFormat(), m_pRenderpassWrapper->GetRenderpass());

	m_pCommandPoolManager->EndSingleTimeCommands(m_Device, commandBuffer, m_GraphicsQueue);
}

VkFormat D3D::VulkanRenderer3D::FindDepthFormat()
{
	return FindSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
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

	auto extent{ m_pSwapchainWrapper->GetExtent() };

	buffer.proj = glm::perspective(glm::radians(45.0f), extent.width / static_cast<float>(extent.height), 0.1f, 100.0f);
	buffer.proj[1][1] *= -1;
}

std::vector<VkDescriptorSetLayout>& D3D::VulkanRenderer3D::GetDescriptorSetLayout(int vertexUbos, int fragmentUbos, int textureAmount)
{
	return m_pPipelineManager->GetDescriptorSetLayout(m_Device, m_MaxFramesInFlight, vertexUbos, fragmentUbos, textureAmount);
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


D3D::QueueFamilyIndices D3D::VulkanRenderer3D::FindQueueFamilies(VkPhysicalDevice physicalDevice)
{
	D3D::QueueFamilyIndices indices;
	
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

	//find at least one queue family that supports VK_QUEUE_GRAPHICS_BIT
	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, m_Surface, &presentSupport);

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

D3D::SwapChainSupportDetails D3D::VulkanRenderer3D::QuerySwapChainSupport(VkPhysicalDevice physicalDevice)
{
	D3D::SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_Surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}