// VulkanRenderer.cpp

// File includes
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
#include "Window.h"

// Standard library includes
#include <set>
#include <algorithm>
#include <iostream>

D3D::VulkanRenderer3D::VulkanRenderer3D()
{
	// Initialize vulkan objects
	InitVulkan();

	// Initialize ImGui
	InitImGui();
}

D3D::VulkanRenderer3D::~VulkanRenderer3D()
{
	// Waint until the logical device isn't doing anything
	vkDeviceWaitIdle(m_Device);

	// Clean up ImGui
	CleanupImGui();

	// Clean up vulkan objects
	CleanupVulkan();
}

void D3D::VulkanRenderer3D::CleanupVulkan()
{
	// Clean up global light
	m_pGlobalLight->Cleanup(m_Device);

	// Clean up swapchain
	m_pSwapchainWrapper->Cleanup(m_Device);

	// Clean up image manager
	m_pImageManager->Cleanup(m_Device);

	// Clean up descriptorpools
	m_pDescriptorPoolManager->Cleanup(m_Device);

	// Clean up graphics pipelines
	m_pPipelineManager->Cleanup(m_Device);

	// Clean up renderpass
	m_pRenderpassWrapper->cleanup(m_Device);

	// Clean up sync objects
	m_pSyncObjectManager->Cleanup(m_Device);

	// Clean up commandpools
	m_pCommandPoolManager->Cleanup(m_Device);

	// Destroy the logical device
	vkDestroyDevice(m_Device, nullptr);

	// Destroy the surface
	vkDestroySurfaceKHR(m_pInstanceWrapper->GetInstance(), m_Surface, nullptr);
}

void D3D::VulkanRenderer3D::CleanupImGui()
{
	// Clean up ImGui
	m_pImGuiWrapper->Cleanup(m_Device);
}

void D3D::VulkanRenderer3D::InitVulkan()
{
	// Initialize the vulkan instance
	m_pInstanceWrapper = std::make_unique<InstanceWrapper>();

	// Initialize the surface
	CreateSurface();
	// Pick the most suitable physical device
	PickPhysicalDevice();
	// Initialize the logical device
	CreateLogicalDevice();

	// Initialize command pool manager
	m_pCommandPoolManager = std::make_unique<CommandpoolManager>(m_Device, m_PhysicalDevice, m_Surface, m_MaxFramesInFlight);

	// Initialize the image manager
	m_pImageManager = std::make_unique<ImageManager>(m_Device, m_PhysicalDevice,
		m_pCommandPoolManager.get(), m_QueueObject.graphicsQueue);

	// Get the max amount of samples per pixel
	auto msaaSamples = VulkanUtils::GetMaxUsableSampleCount(m_PhysicalDevice);

	// Initialize the swapchain
	m_pSwapchainWrapper = std::make_unique<SwapchainWrapper>(m_Device, m_PhysicalDevice, m_Surface,
		m_pImageManager.get(), msaaSamples);

	// Initialize the renderpass
	m_pRenderpassWrapper = std::make_unique<RenderpassWrapper>(m_Device, m_pSwapchainWrapper->GetFormat(), VulkanUtils::FindDepthFormat(m_PhysicalDevice), msaaSamples);

	// Create a single time command buffer
	auto commandBuffer{ m_pCommandPoolManager->BeginSingleTimeCommands(m_Device) };
	// Initialize swapchain
	m_pSwapchainWrapper->SetupImageViews(m_Device, m_PhysicalDevice, m_pImageManager.get(),
		commandBuffer, m_pRenderpassWrapper->GetRenderpass());
	// End the single time command buffer
	m_pCommandPoolManager->EndSingleTimeCommands(m_Device, commandBuffer, m_QueueObject.graphicsQueue);

	// Create global light
	m_pGlobalLight = std::make_unique<DirectionalLightObject>(this);
	// Set direction of global light
	m_pGlobalLight->SetDirection(glm::vec3{-.577, -.577f, .577});
	// Set color of global light
	m_pGlobalLight->SetColor(glm::vec3{1.f, 1.f, 1.f} );
	// Set intensity of global light
	m_pGlobalLight->SetIntensity(1.f);

	// Initialize descriptorpools
	m_pDescriptorPoolManager = std::make_unique<DescriptorPoolManager>();

	// Initialize graphics pipeline manager
	m_pPipelineManager = std::make_unique<PipelineManager>(m_DefaultPipelineName);

	// Add the default graphics pipeline
	AddGraphicsPipeline(m_DefaultPipelineName, m_DefaultVertName, m_DefaultFragName, 1, 1, 0);

	// Initialize the sync objects
	m_pSyncObjectManager = std::make_unique<SyncObjectManager>(m_Device, m_MaxFramesInFlight);
}

void D3D::VulkanRenderer3D::InitImGui()
{
	// Create ImGui vulkan init info
	ImGui_ImplVulkan_InitInfo init_info = {};
	// Give the vulkan instance
	init_info.Instance = m_pInstanceWrapper->GetInstance();
	// Give the physical device
	init_info.PhysicalDevice = m_PhysicalDevice;
	// Give the logical device
	init_info.Device = m_Device;
	// Give the index of the graphics queue family
	init_info.QueueFamily = m_QueueObject.graphicsQueueIndex;
	// Give the graphics queue
	init_info.Queue = m_QueueObject.graphicsQueue;
	// Set pipeline cache to null handle
	init_info.PipelineCache = VK_NULL_HANDLE;
	// Set Allocator to null handle
	init_info.Allocator = VK_NULL_HANDLE;
	// Set min image count to the minimum image count of the swapchain
	init_info.MinImageCount = m_pSwapchainWrapper->GetMinImageCount();
	// Set the image count to the max amount of frames in flight
	init_info.ImageCount = m_MaxFramesInFlight;
	// Give functoin for error handling
	init_info.CheckVkResultFn = [](VkResult /*err*/) { /* error handling */ };
	// Give the max amount of samples per mixel
	init_info.MSAASamples = m_pSwapchainWrapper->GetMsaaSamples();

	// Create a single time command buffer
	auto commandBuffer{ BeginSingleTimeCommands() };
	// Initialize ImGui
	m_pImGuiWrapper = std::make_unique<D3D::ImGuiWrapper>(init_info, m_pRenderpassWrapper->GetRenderpass(), commandBuffer, m_Device, m_MaxFramesInFlight);
	// End the single time command buffer
	EndSingleTimeCommands(commandBuffer);
}

void D3D::VulkanRenderer3D::AddGraphicsPipeline(const std::string& pipelineName, const std::string& vertShaderName, const std::string& fragShaderName, int vertexUbos, int fragmentUbos, int textureAmount)
{
	// Add a graphics pipeline trough the pipeline manager
	m_pPipelineManager->AddGraphicsPipeline(m_Device, m_MaxFramesInFlight, m_pRenderpassWrapper->GetRenderpass(),
		m_pSwapchainWrapper->GetMsaaSamples(), pipelineName, vertShaderName, fragShaderName,
		vertexUbos, fragmentUbos, textureAmount);
}

void D3D::VulkanRenderer3D::Render(std::vector<std::unique_ptr<Model>>& pModels)
{
	// Wait for the in flight fence of the current frame
	vkWaitForFences(m_Device, 1, &m_pSyncObjectManager->GetInFlightFence(m_CurrentFrame), VK_TRUE, UINT64_MAX);

	// Create image index uint
	uint32_t imageIndex{};
	// Get the index of the next image
	VkResult result = vkAcquireNextImageKHR(m_Device, m_pSwapchainWrapper->GetSwapchain(), UINT64_MAX, m_pSyncObjectManager->GetImageAvailableSemaphore(m_CurrentFrame), VK_NULL_HANDLE, &imageIndex);

	// Check if window is out of date
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		// Recreate swapchain if window should be resized
		RecreateSwapChain();
		return;
	}
	// If acquire image was not successful, throw runtime error
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	// Reset the in flight fences
	vkResetFences(m_Device, 1, &m_pSyncObjectManager->GetInFlightFence(m_CurrentFrame));

	// Get the current command buffer
	auto commandBuffer{GetCurrentCommandBuffer()};

	// Reset the command buffer
	vkResetCommandBuffer(commandBuffer, 0);

	// Record the correct command buffer
	RecordCommandBuffer(commandBuffer, imageIndex, pModels);

	// Create submit info object
	VkSubmitInfo submitInfo{};
	// Set type to submit info
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	// Create array for image availabel semaphores
	VkSemaphore waitSemaphores[] = { m_pSyncObjectManager->GetImageAvailableSemaphore(m_CurrentFrame)};
	// Create array for the pipeline stage flags and set it to color attachment output
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	// Set semaphore count to 1
	submitInfo.waitSemaphoreCount = 1;
	// Give array of semaphores
	submitInfo.pWaitSemaphores = waitSemaphores;
	// Give array of wait stages
	submitInfo.pWaitDstStageMask = waitStages;
	// Set the command buffer count to 1
	submitInfo.commandBufferCount = 1;
	// Give pointer to the correct command buffer
	submitInfo.pCommandBuffers = &commandBuffer;

	// Create array for render finished semaphores
	VkSemaphore signalSemaphores[] = { m_pSyncObjectManager->GetRenderFinishedSemaphore(m_CurrentFrame) };
	// Set signal semaphore count to 1
	submitInfo.signalSemaphoreCount = 1;
	// Give array of signal semaphores
	submitInfo.pSignalSemaphores = signalSemaphores;

	// Submit the command buffers
	if (vkQueueSubmit(m_QueueObject.graphicsQueue, 1, &submitInfo, m_pSyncObjectManager->GetInFlightFence(m_CurrentFrame)) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	// Create present info object
	VkPresentInfoKHR presentInfo{};
	// Set type to present info
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	// Set wait semaphore count to 1
	presentInfo.waitSemaphoreCount = 1;
	// Give the array of signal semaphores
	presentInfo.pWaitSemaphores = signalSemaphores;

	// Create array of swapchains and add the current swapchaint
	VkSwapchainKHR swapChains[] = { m_pSwapchainWrapper->GetSwapchain()};
	// Set swapchain count to 1
	presentInfo.swapchainCount = 1;
	// Give array of swapchains
	presentInfo.pSwapchains = swapChains;
	// Give pointer to image index of current frame
	presentInfo.pImageIndices = &imageIndex;
	// Set pResults to nullptr
	presentInfo.pResults = nullptr;

	// Present the swapchain
	result = vkQueuePresentKHR(m_QueueObject.presentQueue, &presentInfo);

	// Check if window was resized and is out of date
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || Window::GetInstance().GetWindowStruct().FrameBufferResized)
	{
		// If necesarry, resize swapchain
		RecreateSwapChain();
		// Reset FrameBufferResized flag
		Window::GetInstance().SetFrameBufferResized(false);
	}
	// Check if presentation of swapchain was successful
	else if (result != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to present swap chain image!");
	}

	// Go to the next frame
	++m_CurrentFrame %= m_MaxFramesInFlight;
}

void D3D::VulkanRenderer3D::RecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex, std::vector<std::unique_ptr<Model>>& pModels)
{
	// Get the swapchain extent
	auto swapchainExtent{ m_pSwapchainWrapper->GetExtent() };

	// Create command buffer begin info object
	VkCommandBufferBeginInfo beginInfo{};
	// Set type to command buffer begin info
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	// Set the flags to render pass continue
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
	// Set inheritance info to nullptr
	beginInfo.pInheritanceInfo = nullptr;

	// Begin the command buffer
	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	// Create renderpass begin info object
	VkRenderPassBeginInfo renderPassInfo{};
	// Set type to render pass begin info
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	// Give handle of renderpass
	renderPassInfo.renderPass = m_pRenderpassWrapper->GetRenderpass();
	// Give the current frame buffer
	renderPassInfo.framebuffer = m_pSwapchainWrapper->GetFrameBuffer(imageIndex);
	// Set offset of render area to 0, 0
	renderPassInfo.renderArea.offset = { 0, 0 };
	// Set extent of render area to the extent of the swapchain
	renderPassInfo.renderArea.extent = swapchainExtent;

	// Create an array for the clear values
	std::array<VkClearValue, 2> clearValues{};
	// Set color to a color of choice
	clearValues[0].color = { {0.388f, 0.588f, 0.929f, 1.0f} };
	// Set depth stencil to 1, 0
	clearValues[1].depthStencil = { 1.0f, 0 };

	// Give the size of the clearValues size as the count for clearvalues
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	// Give pointer to the clear values data
	renderPassInfo.pClearValues = clearValues.data();

	// Begin the renderpass
	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	// Create a viewport object
	VkViewport viewport{};
	// Set viewport x to 0
	viewport.x = 0.0f;
	// Set viewport y to 0
	viewport.y = 0.0f;
	// Set the viewport width to the width of the swapchain
	viewport.width = static_cast<float>(swapchainExtent.width);
	// Set the viewport width to the width of the swapchain
	viewport.height = static_cast<float>(swapchainExtent.height);
	// Set min depth to 0
	viewport.minDepth = 0.0f;
	// Set max depth to 1
	viewport.maxDepth = 1.0f;

	// Set the viewport
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	// Create scissor object
	VkRect2D scissor{};
	// Set offset to 0, 0
	scissor.offset = { 0, 0 };
	// Set scissor extent to swapchain extent
	scissor.extent = swapchainExtent;

	// Set the scissor
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	// Update the buffer of the global light
	m_pGlobalLight->UpdateBuffer(m_CurrentFrame);

	// Loop trough the amount of models
	for (size_t i = 0; i < pModels.size(); ++i)
	{
		// Render the current model
		pModels[i]->Render();
	}

	// Render the ImGui
	m_pImGuiWrapper->Render(commandBuffer);

	// End the render pass
	vkCmdEndRenderPass(commandBuffer);

	// End the command buffer
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to record command buffer!");
	}
}

void D3D::VulkanRenderer3D::Render(Model* pModel, VkCommandBuffer& commandBuffer, const VkDescriptorSet* descriptorSet, const PipelinePair& pipeline)
{
	// Bind the correct pipeline
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline);

	//vkCmdPushConstants(commandBuffer, pipeline.pipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(LightObject), &m_GlobalLight);

	// Create array for vertexbuffer and get correct vertex buffer
	VkBuffer vertexBuffers[] = { pModel->GetVertexBuffer() };
	// Set the offset to 0
	VkDeviceSize offsets[] = { 0 };
	// Bind the vertex buffer
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	// Bind the index buffer
	vkCmdBindIndexBuffer(commandBuffer, pModel->GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

	// Bind the descriptor sets
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout, 0, 1, descriptorSet, 0, nullptr);

	// Draw the current model
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(pModel->GetIndexAmount()), 1, 0, 0, 0);
}

VkImageView& D3D::VulkanRenderer3D::GetDefaultImageView()
{
	// Return the default image view trough the image manager
	return m_pImageManager->GetDefaultImageView();
}

VkSampler& D3D::VulkanRenderer3D::GetSampler()
{
	// Return the default sampler trough the image manager
	return m_pImageManager->GetSampler();
}

D3D::PipelinePair& D3D::VulkanRenderer3D::GetPipeline(const std::string& name)
{
	// Return the requested pipeline trough the pipeline manager
	return m_pPipelineManager->GetPipeline(name);
}

VkCommandBuffer& D3D::VulkanRenderer3D::GetCurrentCommandBuffer()
{
	// Return the requested command buffer trough the commandpool manager
	return m_pCommandPoolManager->GetCommandBuffer(m_CurrentFrame);
}

D3D::DescriptorPoolManager* D3D::VulkanRenderer3D::GetDescriptorPoolManager() const
{
	// Get the descriptor pool manager
	return m_pDescriptorPoolManager.get();
}

const D3D::DirectionalLightStruct& D3D::VulkanRenderer3D::GetGlobalLight() const
{
	// Get a reference to the global light struct
	return m_pGlobalLight->GetLight();
}

std::vector<VkBuffer>& D3D::VulkanRenderer3D::GetLightBuffers()
{
	// Return buffers of the global light object
	return m_pGlobalLight->GetLightBuffers();
}

void D3D::VulkanRenderer3D::CreateSurface()
{
	// Create the window surface
	if (glfwCreateWindowSurface(m_pInstanceWrapper->GetInstance(), Window::GetInstance().GetWindowStruct().pWindow, nullptr, &m_Surface) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
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

	// Loop trough all available physical devices
	for (const auto& device : devices)
	{
		// Check if the device is suitable
		if (IsDeviceSuitable(device))
		{
			// Set the physical device to the current one and break the loop
			m_PhysicalDevice = device;
			break;
		}
	}

	// Check if physical device is null handle
	if (m_PhysicalDevice == VK_NULL_HANDLE)
	{
		// If null handle, throw runtime error
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

bool D3D::VulkanRenderer3D::IsDeviceSuitable(VkPhysicalDevice device)
{
	// Get the queue families for the given physical device and surface
	QueueFamilyIndices indices = VulkanUtils::FindQueueFamilies(device, m_Surface);

	// Check if the device supports all requested extensions
	bool extensionsSupported = CheckDeviceExtensionSupport(device);

	// Boolean for adequaty of the swapchain
	bool swapChainAdequate = false;

	// If the extensions are supported
	if (extensionsSupported)
	{
		// Get the swapchain support details
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device, m_Surface);
		// If formats and presentmodes are not empty, set swapChainAdequate to true
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	// Get the physical device features
	VkPhysicalDeviceFeatures supportedFeatures{};
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	// Return true if:
	// The device queue families are complete
	// The extensions are all supported
	// The swapchain is adequate
	// Sampler anisotropy is supported
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
}


void D3D::VulkanRenderer3D::CreateLogicalDevice()
{
	// Get the suited queue family indices
	QueueFamilyIndices indices = VulkanUtils::FindQueueFamilies(m_PhysicalDevice, m_Surface);

	// Create vector for queue create infos
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	// Create vector for the unique families and initialize with the graphics- and present family queues
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	// Set the graphics family queue to the value of the graphics family index
	m_QueueObject.graphicsQueueIndex = indices.graphicsFamily.value();

	// Initialize queuePriority with 1
	float queuePriority = 1.0f;
	// Loop trough all the unique families
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		// Create queue create info
		VkDeviceQueueCreateInfo queueCreateInfo{};
		// Set type to queue create info
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		// Set queue family index to the current queue family
		queueCreateInfo.queueFamilyIndex = queueFamily;
		// Set queue count to 1
		queueCreateInfo.queueCount = 1;
		// Give the queue priority
		queueCreateInfo.pQueuePriorities = &queuePriority;
		// Place the create info in the vector of create infos
		queueCreateInfos.push_back(queueCreateInfo);
	}

	// Initialize device features object
	VkPhysicalDeviceFeatures deviceFeatures{};
	// Enable anisotropy sampler
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	// Enable sampler rate shading
	deviceFeatures.sampleRateShading = VK_TRUE;

	// Create device create info
	VkDeviceCreateInfo createInfo{};
	// Set type to device create info
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	// Set the amount of queue create infos to the size of the vector of create infos
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	// Give pointer to data of queue create info vector
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	// Give the requested device features
	createInfo.pEnabledFeatures = &deviceFeatures;
	// Set amount of extensions to the size of the extensions vector
	createInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
	// Give pointer to data of extensions vector
	createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

	// Check if validation layers are enabled
	if (m_pInstanceWrapper->ValidationLayersEnabled())
	{   // If enabled
		// Set the amount of validation layers as the size of the validation layer vector
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_pInstanceWrapper->GetValidationLayers().size());
		// Give pointer to data of the validation layer vector
		createInfo.ppEnabledLayerNames = m_pInstanceWrapper->GetValidationLayers().data();
	}
	else
	{
		// If not enabled, set layer count to 0
		createInfo.enabledLayerCount = 0;
	}

	// Create the logical device
	if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to create logical device!");
	}

	// Get the graphics queue
	vkGetDeviceQueue(m_Device, indices.graphicsFamily.value(), 0, &m_QueueObject.graphicsQueue);
	// Get the present queue
	vkGetDeviceQueue(m_Device, indices.presentFamily.value(), 0, &m_QueueObject.presentQueue);
}


void D3D::VulkanRenderer3D::RecreateSwapChain()
{
	// Get a reference to the window struct
	auto& windowStruct{ Window::GetInstance().GetWindowStruct() };

	//Get the width and the height of the window
	glfwGetFramebufferSize(windowStruct.pWindow, &windowStruct.Width, &windowStruct.Height);

	// While width and height are 0, wait before continuing
	while (windowStruct.Width == 0 || windowStruct.Height == 0)
	{
		glfwGetFramebufferSize(windowStruct.pWindow, &windowStruct.Width, &windowStruct.Height);
		glfwWaitEvents();
	}

	// Wait until the device is idle
	vkDeviceWaitIdle(m_Device);
	
	// Create a single time command
	auto commandBuffer{ BeginSingleTimeCommands() };

	// Recreate the swapchain
	m_pSwapchainWrapper->RecreateSwapChain(m_Device, m_PhysicalDevice, m_Surface, m_pImageManager.get(),
		commandBuffer, m_pRenderpassWrapper->GetRenderpass());

	// End single time command
	EndSingleTimeCommands(commandBuffer);
}


void D3D::VulkanRenderer3D::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)
{
	// Begin a single time command buffer
	VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

	// Transition the image trough the image manager
	m_pImageManager->TransitionImageLayout(image, commandBuffer, format, oldLayout, newLayout, mipLevels);

	// End the single time command buffer
	EndSingleTimeCommands(commandBuffer);
}

void D3D::VulkanRenderer3D::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	// Create the buffer trough vulkan utils
	VulkanUtils::CreateBuffer(m_Device, m_PhysicalDevice, size, usage, properties, buffer, bufferMemory);
}

void D3D::VulkanRenderer3D::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	// Begin a single time command buffer
	VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

	// Create a buffer copy region
	VkBufferCopy copyRegion{};
	// Set size to the size of the buffer
	copyRegion.size = size;

	// Copy the buffer
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	// End the single time command
	EndSingleTimeCommands(commandBuffer);
}

void D3D::VulkanRenderer3D::UpdateUniformBuffer(UniformBufferObject& buffer)
{
	// Create camera position vector
	glm::vec3 cameraPosition(0.0f, 0.0f, 0.0f);
	// Create camera scale vector
	glm::vec3 cameraScale(1.0f, 1.0f, 1.0f);
	// Create camera rotation vector
	glm::vec3 cameraRotation(0.0f, 0.0f, 0.0f);

	// Set view to identity matrix
	buffer.view = glm::mat4{ 1.0f };

	// Apply translation
	buffer.view = glm::translate(buffer.view, cameraPosition);

	// Apply scaling
	buffer.view = glm::scale(buffer.view, cameraScale);

	// Apply rotation
	buffer.view = glm::rotate(buffer.view, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// Get the swapchain extent
	auto extent{ m_pSwapchainWrapper->GetExtent() };

	// Set the projection matrix
	buffer.proj = glm::perspective(glm::radians(45.0f), extent.width / static_cast<float>(extent.height), 0.1f, 100.0f);
	buffer.proj[1][1] *= -1;
}

std::vector<VkDescriptorSetLayout>& D3D::VulkanRenderer3D::GetDescriptorSetLayout(int vertexUbos, int fragmentUbos, int textureAmount)
{
	// Return the requested descriptor set layout trough the pipeline manager
	return m_pPipelineManager->GetDescriptorSetLayout(m_Device, m_MaxFramesInFlight, vertexUbos, fragmentUbos, textureAmount);
}

void D3D::VulkanRenderer3D::CreateTexture(Texture& texture, const std::string& textureName, uint32_t& mipLevels)
{
	// Create the image trough the image manager
	m_pImageManager->CreateTextureImage(m_Device, m_PhysicalDevice, texture, textureName, mipLevels, m_pCommandPoolManager.get(), m_QueueObject.graphicsQueue);
	// Create the image view
	texture.imageView = m_pImageManager->CreateImageView(m_Device, texture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);
}

VkCommandBuffer D3D::VulkanRenderer3D::BeginSingleTimeCommands()
{
	// Create a single time command buffer trough the command pool manager and return it
	return m_pCommandPoolManager->BeginSingleTimeCommands(m_Device);
}

void D3D::VulkanRenderer3D::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
{
	// End the single time command buffer trough the commandpool manager
	m_pCommandPoolManager->EndSingleTimeCommands(m_Device, commandBuffer, m_QueueObject.graphicsQueue);
}

D3D::SwapChainSupportDetails D3D::VulkanRenderer3D::QuerySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
	// Create swapchainsupport details object
	D3D::SwapChainSupportDetails details;

	// Get surface capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

	// Create formatcount uint
	uint32_t formatCount;
	// Get surface formats
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

	// If there is more than 0 image counts
	if (formatCount != 0)
	{
		// Resize formats to the amount of formats
		details.formats.resize(formatCount);
		// Geth the surface formats
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());
	}

	// Create present mode count
	uint32_t presentModeCount;
	// Get amount of present modes
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

	// If there is more than 0 present modes
	if (presentModeCount != 0)
	{
		// Resize present modes to amount of present modes
		details.presentModes.resize(presentModeCount);
		// Get the surface present modes
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.presentModes.data());
	}

	// Return swapchain support details
	return details;
}