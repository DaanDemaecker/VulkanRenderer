#include "VulkanRenderer3D.h"
#include "D3DEngine.h"
#include "Utils.h"
#include "Model.h"
#include "VulkanUtils.h"

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
		m_pCommandPoolManager.get(), m_QueueObject.graphicsQueue);

	auto msaaSamples = VulkanUtils::GetMaxUsableSampleCount(m_PhysicalDevice);

	m_pSwapchainWrapper = std::make_unique<SwapchainWrapper>(m_Device, m_PhysicalDevice, m_Surface,
		m_pImageManager.get(), msaaSamples);

	m_pRenderpassWrapper = std::make_unique<RenderpassWrapper>(m_Device, m_pSwapchainWrapper->GetFormat(), VulkanUtils::FindDepthFormat(m_PhysicalDevice), msaaSamples);

	auto commandBuffer{ m_pCommandPoolManager->BeginSingleTimeCommands(m_Device) };
	m_pSwapchainWrapper->SetupImageViews(m_Device, m_PhysicalDevice, m_pImageManager.get(),
		commandBuffer, m_pRenderpassWrapper->GetRenderpass());
	m_pCommandPoolManager->EndSingleTimeCommands(m_Device, commandBuffer, m_QueueObject.graphicsQueue);


	m_pGlobalLight = std::make_unique<DirectionalLightObject>(this);
	m_pGlobalLight->SetDirection(glm::vec3{-.577, -.577f, .577});
	m_pGlobalLight->SetColor(glm::vec3{1.f, 1.f, 1.f} );
	m_pGlobalLight->SetIntensity(1.f);


	m_pDescriptorPoolManager = std::make_unique<DescriptorPoolManager>();

	m_pPipelineManager = std::make_unique<PipelineManager>(m_DefaultPipelineName);

	AddGraphicsPipeline(m_DefaultPipelineName, m_DefaultVertName, m_DefaultFragName, 1, 1, 0);

	m_pSyncObjectManager = std::make_unique<SyncObjectManager>(m_Device, m_MaxFramesInFlight);
}

void D3D::VulkanRenderer3D::InitImGui()
{
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = m_pInstanceWrapper->GetInstance(); // Your Vulkan instance
	init_info.PhysicalDevice = m_PhysicalDevice; // Your Vulkan physical device
	init_info.Device = m_Device; // Your Vulkan logical device
	init_info.QueueFamily = m_QueueObject.graphicsQueueIndex; // The index of your Vulkan queue family that supports graphics operations
	init_info.Queue = m_QueueObject.graphicsQueue; // Your Vulkan graphics queue
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

	if (vkQueueSubmit(m_QueueObject.graphicsQueue, 1, &submitInfo, m_pSyncObjectManager->GetInFlightFence(m_CurrentFrame)) != VK_SUCCESS)
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

	result = vkQueuePresentKHR(m_QueueObject.presentQueue, &presentInfo);

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
	QueueFamilyIndices indices = VulkanUtils::FindQueueFamilies(device, m_Surface);

	bool extensionsSupported = CheckDeviceExtensionSupport(device);

	bool swapChainAdequate = false;

	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device, m_Surface);
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
	QueueFamilyIndices indices = VulkanUtils::FindQueueFamilies(m_PhysicalDevice, m_Surface);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	m_QueueObject.graphicsQueueIndex = indices.graphicsFamily.value();

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

	vkGetDeviceQueue(m_Device, indices.graphicsFamily.value(), 0, &m_QueueObject.graphicsQueue);
	vkGetDeviceQueue(m_Device, indices.presentFamily.value(), 0, &m_QueueObject.presentQueue);
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

	m_pSwapchainWrapper->RecreateSwapChain(m_Device, m_PhysicalDevice, m_Surface, m_pImageManager.get(),
		commandBuffer, m_pRenderpassWrapper->GetRenderpass());

	m_pCommandPoolManager->EndSingleTimeCommands(m_Device, commandBuffer, m_QueueObject.graphicsQueue);
}


void D3D::VulkanRenderer3D::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)
{
	VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

	m_pImageManager->TransitionImageLayout(image, commandBuffer, format, oldLayout, newLayout, mipLevels);

	EndSingleTimeCommands(commandBuffer);
}

void D3D::VulkanRenderer3D::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	VulkanUtils::CreateBuffer(m_Device, m_PhysicalDevice, size, usage, properties, buffer, bufferMemory);
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
	m_pImageManager->CreateTextureImage(m_Device, m_PhysicalDevice, texture, textureName, mipLevels, m_pCommandPoolManager.get(), m_QueueObject.graphicsQueue);
	texture.imageView = m_pImageManager->CreateImageView(m_Device, texture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);
}

VkCommandBuffer D3D::VulkanRenderer3D::BeginSingleTimeCommands()
{
	return m_pCommandPoolManager->BeginSingleTimeCommands(m_Device);
}

void D3D::VulkanRenderer3D::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
{
	m_pCommandPoolManager->EndSingleTimeCommands(m_Device, commandBuffer, m_QueueObject.graphicsQueue);
}

D3D::SwapChainSupportDetails D3D::VulkanRenderer3D::QuerySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
	D3D::SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}