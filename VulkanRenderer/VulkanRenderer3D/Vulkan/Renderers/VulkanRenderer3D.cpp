// VulkanRenderer.cpp

// File includes
#include "VulkanRenderer3D.h"

#include "Vulkan/Vulkan3D.h"
#include "Engine/ConfigManager.h"

#include "Vulkan/Managers/DispatchableManager.h"
#include "Vulkan/Wrappers/GPUObject.h"

#include "Vulkan/Managers/PipelineManager.h"
#include "Vulkan/Wrappers/RenderpassWrapper.h"
#include "Vulkan/Wrappers/SwapchainWrapper.h"
#include "Vulkan/Managers/ImageManager.h"
#include "Vulkan/Managers/SyncObjectManager.h"
#include "Vulkan/Wrappers/ImGuiWrapper.h"
#include "Vulkan/VulkanUtils.h"
#include "Engine/Window.h"
#include "DataTypes/RenderClasses/Model.h"
#include "Vulkan/Managers/BufferManager.h"
#include "Vulkan/Wrappers/DescriptorPoolWrapper.h"
#include "Vulkan/Wrappers/PipelineWrapper.h"
#include "Vulkan/Managers/CommandpoolManager.h"
#include "Vulkan/Wrappers/SurfaceWrapper.h"
#include "Vulkan/Wrappers/Viewport.h"
#include "ShadowRenderer.h"

#include "DataTypes/Camera.h"
#include "DataTypes/DirectionalLightObject.h"
#include "DataTypes/RenderClasses/SkyBox.h"

// Standard library includes
#include <set>
#include <algorithm>
#include <iostream>

D3D::VulkanRenderer3D::VulkanRenderer3D()
{
	// Create camera
	m_pCamera = std::make_unique<Camera>();

	// Set initial rotation
	m_pCamera->SetRotation(glm::vec3(0.0f, glm::radians(180.f), 0.0f));

	// Initialize vulkan objects
	InitVulkan();

	// Initialize ImGui
	InitImGui();
}

D3D::VulkanRenderer3D::~VulkanRenderer3D()
{
	// Waint until the logical device isn't doing anything
	vkDeviceWaitIdle(Vulkan3D::GetInstance().GetDevice());

	// Clean up vulkan objects
	CleanupVulkan();
}

void D3D::VulkanRenderer3D::SetupSkybox()
{
	// Get config manager
	auto& configManager{ ConfigManager::GetInstance() };
	// Create the graphics pipeline for the skybox
	AddGraphicsPipeline("Skybox", { configManager.GetString("SkyboxVert"), configManager.GetString("SkyboxFrag")}, false);

	// Create the skybox
	m_pSkyBox = std::make_unique<SkyBox>(
		std::initializer_list<const std::string>{"resources/images/CubeMap/Sky_Right.png",
												"resources/images/CubeMap/Sky_Left.png",
												"resources/images/CubeMap/Sky_Up.png", 
												"resources/images/CubeMap/Sky_Down.png", 
												"resources/images/CubeMap/Sky_Front.png", 
												"resources/images/CubeMap/Sky_Back.png"});

}

void D3D::VulkanRenderer3D::CleanupSkybox()
{
	// Set the skybox to nullptr, the unique pointer will destroy it
	m_pSkyBox = nullptr;
}

void D3D::VulkanRenderer3D::SetupLight()
{
	// Create global light
	m_pGlobalLight = std::make_unique<DirectionalLightObject>();
	// Set direction of global light
	m_pGlobalLight->SetDirection(glm::vec3{ 1.f, -1.f, 1.f });
	// Set color of global light
	m_pGlobalLight->SetColor(glm::vec3{ 1.f, 1.f, 1.f });
	// Set intensity of global light
	m_pGlobalLight->SetIntensity(1.f);
}

void D3D::VulkanRenderer3D::SetupDefaultPipeline()
{
	auto device{ Vulkan3D::GetInstance().GetDevice() };

	m_pShadowRenderer->CreatePipeline(device);
	// Add the default pipeline
	m_pPipelineManager->AddDefaultPipeline(device, m_pRenderpassWrapper->GetRenderpass(), m_pSwapchainWrapper->GetMsaaSamples());
}

D3D::TextureDescriptorObject* D3D::VulkanRenderer3D::GetShadowMapDescriptorObject()
{
	return m_pShadowRenderer->GetTextureDescriptorObject();
}

void D3D::VulkanRenderer3D::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, uint32_t layerCount)
{
	auto commandBuffer{ m_pCommandPoolManager->BeginSingleTimeCommands(Vulkan3D::GetInstance().GetDevice()) };
	m_pImageManager->TransitionImageLayout(image, commandBuffer, format, oldLayout, newLayout, mipLevels, layerCount);
	m_pCommandPoolManager->EndSingleTimeCommands(Vulkan3D::GetInstance().GetGPUObject(), commandBuffer);
}

void D3D::VulkanRenderer3D::CleanupLight()
{
	// Set the global light to nullptr, the unique pointer will take care of the destruction
	m_pGlobalLight = nullptr;
}

void D3D::VulkanRenderer3D::CleanupVulkan()
{
	// Get handle to logical device
	auto device{ Vulkan3D::GetInstance().GetDevice()};

	// Clean up image manager
	m_pImageManager->Cleanup(device);

	// Clean up graphics pipelines
	m_pPipelineManager->Cleanup(device);

	// Clean up renderpass
	m_pRenderpassWrapper->cleanup(device);

	// Clean up sync objects
	m_pSyncObjectManager->Cleanup(device);

	// Clean up commandpools
	m_pCommandPoolManager->Cleanup(device);
}

void D3D::VulkanRenderer3D::InitVulkan()
{
	// Create buffer manager
	m_pBufferManager = std::make_unique<BufferManager>();

	auto surface{ Vulkan3D::GetInstance().GetSurface()};

	// Get pointer to gpu object
	GPUObject* pGPUObject{ Vulkan3D::GetInstance().GetGPUObject()};

	// Initialize command pool manager
	m_pCommandPoolManager = std::make_unique<CommandpoolManager>(pGPUObject, surface);

	// Initialize the image manager
	m_pImageManager = std::make_unique<ImageManager>(pGPUObject, m_pBufferManager.get(), m_pCommandPoolManager.get());

	// Get the max amount of samples per pixel
	auto msaaSamples = VulkanUtils::GetMaxUsableSampleCount(pGPUObject->GetPhysicalDevice());

	// Initialize the swapchain
	m_pSwapchainWrapper = std::make_unique<SwapchainWrapper>(pGPUObject, surface,	m_pImageManager.get(), msaaSamples);

	// Initialize the renderpass
	m_pRenderpassWrapper = std::make_unique<RenderpassWrapper>(pGPUObject->GetDevice(), m_pSwapchainWrapper->GetFormat(), VulkanUtils::FindDepthFormat(), msaaSamples);

	// Create a single time command buffer
	auto commandBuffer{ BeginSingleTimeCommands()};
	// Initialize swapchain
	m_pSwapchainWrapper->SetupImageViews(pGPUObject, m_pImageManager.get(),commandBuffer, m_pRenderpassWrapper->GetRenderpass());
	// End the single time command buffer
	EndSingleTimeCommands(commandBuffer);

	// Initialize graphics pipeline manager
	m_pPipelineManager = std::make_unique<PipelineManager>();

	// Initialize the sync objects
	m_pSyncObjectManager = std::make_unique<SyncObjectManager>(pGPUObject->GetDevice());

	m_pViewport = std::make_unique<Viewport>(m_pSwapchainWrapper->GetExtent());

	m_pShadowRenderer = std::make_unique<ShadowRenderer>();
}

void D3D::VulkanRenderer3D::InitImGui()
{
	// Get pointer to gpu object
	GPUObject* pGPUObject{ D3D::Vulkan3D::GetInstance().GetGPUObject()};

	// Create ImGui vulkan init info
	ImGui_ImplVulkan_InitInfo init_info = {};
	// Give the vulkan instance
	init_info.Instance = D3D::Vulkan3D::GetInstance().GetVulkanInstance();
	// Give the physical device
	init_info.PhysicalDevice = pGPUObject->GetPhysicalDevice();
	// Give the logical device
	init_info.Device = pGPUObject->GetDevice();
	// Give the index of the graphics queue family
	init_info.QueueFamily = pGPUObject->GetQueueObject().graphicsQueueIndex;
	// Give the graphics queue
	init_info.Queue = pGPUObject->GetQueueObject().graphicsQueue;
	// Set pipeline cache to null handle
	init_info.PipelineCache = VK_NULL_HANDLE;
	// Set Allocator to null handle
	init_info.Allocator = VK_NULL_HANDLE;
	// Set min image count to the minimum image count of the swapchain
	init_info.MinImageCount = m_pSwapchainWrapper->GetMinImageCount();
	// Set the image count to the max amount of frames in flight
	init_info.ImageCount = Vulkan3D::GetMaxFrames();
	// Give functoin for error handling
	init_info.CheckVkResultFn = [](VkResult /*err*/) { /* error handling */ };
	// Give the max amount of samples per mixel
	init_info.MSAASamples = m_pSwapchainWrapper->GetMsaaSamples();
	// Set renderpass
	init_info.RenderPass = m_pRenderpassWrapper->GetRenderpass();

	// Initialize ImGui
	m_pImGuiWrapper = std::make_unique<D3D::ImGuiWrapper>(init_info, pGPUObject->GetDevice());
}

void D3D::VulkanRenderer3D::AddGraphicsPipeline(const std::string& pipelineName, std::initializer_list<const std::string>&& filePaths, bool hasDepthStencil)
{
	// Add a graphics pipeline trough the pipeline manager
	m_pPipelineManager->AddGraphicsPipeline(D3D::Vulkan3D::GetInstance().GetDevice(), m_pRenderpassWrapper->GetRenderpass(),
		m_pSwapchainWrapper->GetMsaaSamples(), pipelineName, filePaths, hasDepthStencil);
}

void D3D::VulkanRenderer3D::Render(std::vector<std::unique_ptr<Model>>& pModels)
{
	// Wait for the in flight fence of the current frame
	vkWaitForFences(D3D::Vulkan3D::GetInstance().GetDevice(), 1, &m_pSyncObjectManager->GetInFlightFence(Vulkan3D::GetCurrentFrame()), VK_TRUE, UINT64_MAX);

	// Create image index uint
	uint32_t imageIndex{};
	// Get the index of the next image
	VkResult result = vkAcquireNextImageKHR(D3D::Vulkan3D::GetInstance().GetDevice(), m_pSwapchainWrapper->GetSwapchain(), UINT64_MAX, m_pSyncObjectManager->GetImageAvailableSemaphore(Vulkan3D::GetCurrentFrame()), VK_NULL_HANDLE, &imageIndex);

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
	vkResetFences(D3D::Vulkan3D::GetInstance().GetDevice(), 1, &m_pSyncObjectManager->GetInFlightFence(Vulkan3D::GetCurrentFrame()));

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
	VkSemaphore waitSemaphores[] = { m_pSyncObjectManager->GetImageAvailableSemaphore(Vulkan3D::GetCurrentFrame())};
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
	VkSemaphore signalSemaphores[] = { m_pSyncObjectManager->GetRenderFinishedSemaphore(Vulkan3D::GetCurrentFrame()) };
	// Set signal semaphore count to 1
	submitInfo.signalSemaphoreCount = 1;
	// Give array of signal semaphores
	submitInfo.pSignalSemaphores = signalSemaphores;

	// Submit the command buffers
	if (vkQueueSubmit(D3D::Vulkan3D::GetInstance().GetGPUObject()->GetQueueObject().graphicsQueue, 1, &submitInfo, m_pSyncObjectManager->GetInFlightFence(Vulkan3D::GetCurrentFrame())) != VK_SUCCESS)
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
	result = vkQueuePresentKHR(D3D::Vulkan3D::GetInstance().GetGPUObject()->GetQueueObject().presentQueue, &presentInfo);

	// Check if window was resized and is out of date
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || Window::GetInstance().GetWindowStruct().FrameBufferResized)
	{
		// If necesarry, resize swapchain
		RecreateSwapChain();

		m_pViewport->SetViewportAndScissor(m_pSwapchainWrapper->GetExtent());

		// Reset FrameBufferResized flag
		Window::GetInstance().SetFrameBufferResized(false);
	}
	// Check if presentation of swapchain was successful
	else if (result != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to present swap chain image!");
	}
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


	m_pShadowRenderer->Render(pModels);

	m_pViewport->SetViewport(commandBuffer);

	m_pRenderpassWrapper->BeginRenderPass(commandBuffer, m_pSwapchainWrapper->GetFrameBuffer(imageIndex), swapchainExtent);

	// Update the buffer of the global light
	m_pGlobalLight->UpdateBuffer(Vulkan3D::GetCurrentFrame());


	if (m_pCamera->GetCameraType() == CameraType::Perspective)
	{
		// Render the skybox
		m_pSkyBox->Render();
	}

	// Loop trough the amount of models
	for (size_t i = 0; i < pModels.size(); ++i)
	{
		// Render the current model
		pModels[i]->Render();
	}

	// Render the ImGui
	m_pImGuiWrapper->StartRender();

	m_pImGuiWrapper->EndRender(commandBuffer);

	// End the render pass
	vkCmdEndRenderPass(commandBuffer);

	// End the command buffer
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to record command buffer!");
	}
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

D3D::PipelineWrapper* D3D::VulkanRenderer3D::GetPipeline(const std::string& name)
{
	// Return the requested pipeline trough the pipeline manager
	return m_pPipelineManager->GetPipeline(name);
}

VkCommandBuffer& D3D::VulkanRenderer3D::GetCurrentCommandBuffer()
{
	// Return the requested command buffer trough the commandpool manager
	return m_pCommandPoolManager->GetCommandBuffer(Vulkan3D::GetCurrentFrame());
}

const D3D::DirectionalLightStruct& D3D::VulkanRenderer3D::GetGlobalLightStruct() const
{
	// Get a reference to the global light struct
	return m_pGlobalLight->GetLight();
}

D3D::DirectionalLightObject* D3D::VulkanRenderer3D::GetGlobalLight() const
{
	return m_pGlobalLight.get();
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
	vkDeviceWaitIdle(D3D::Vulkan3D::GetInstance().GetDevice());
	
	// Create a single time command
	auto commandBuffer{ BeginSingleTimeCommands() };

	// Recreate the swapchain
	m_pSwapchainWrapper->RecreateSwapChain(D3D::Vulkan3D::GetInstance().GetGPUObject(), D3D::Vulkan3D::GetInstance().GetSurface(), m_pImageManager.get(),
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
	m_pBufferManager->CreateBuffer(D3D::Vulkan3D::GetInstance().GetGPUObject(), size, usage, properties, buffer, bufferMemory);
}

void D3D::VulkanRenderer3D::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	// Copy a buffer trough the bufferManager
	m_pBufferManager->CopyBuffer(D3D::Vulkan3D::GetInstance().GetGPUObject(), m_pCommandPoolManager.get(), srcBuffer, dstBuffer, size);
}

void D3D::VulkanRenderer3D::CreateVertexBuffer(std::vector<D3D::Vertex>& vertices, VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory)
{
	// Create a vertex buffer trough the buffer manager
	m_pBufferManager->CreateVertexBuffer(D3D::Vulkan3D::GetInstance().GetGPUObject(), m_pCommandPoolManager.get(), vertices, vertexBuffer, vertexBufferMemory);
}

void D3D::VulkanRenderer3D::CreateIndexBuffer(std::vector<uint32_t>& indices, VkBuffer& indexBuffer, VkDeviceMemory& indexBufferMemory)
{
	// Create an index buffer trough the buffer manager
	m_pBufferManager->CreateIndexBuffer(D3D::Vulkan3D::GetInstance().GetGPUObject(), m_pCommandPoolManager.get(), indices, indexBuffer, indexBufferMemory);
}

void D3D::VulkanRenderer3D::UpdateUniformBuffer(UniformBufferObject& buffer)
{
	// Update the buffer with the camera transformation
	m_pCamera->UpdateUniformBuffer(buffer, m_pSwapchainWrapper->GetExtent());
}

void D3D::VulkanRenderer3D::CreateTexture(Texture& texture, const std::string& textureName)
{
	// Create the image trough the image manager
	m_pImageManager->CreateTextureImage(D3D::Vulkan3D::GetInstance().GetGPUObject(), m_pBufferManager.get(), texture, textureName, m_pCommandPoolManager.get());
	// Create the image view
	texture.imageView = m_pImageManager->CreateImageView(D3D::Vulkan3D::GetInstance().GetDevice(), texture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, texture.mipLevels);
}

void D3D::VulkanRenderer3D::CreateCubeTexture(Texture& cubeTexture, const std::initializer_list<const std::string>& textureNames)
{
	// Create a cube texture trough image manager
	m_pImageManager->CreateCubeTexture(D3D::Vulkan3D::GetInstance().GetGPUObject(), m_pBufferManager.get(), cubeTexture, textureNames, m_pCommandPoolManager.get());
}

VkCommandBuffer D3D::VulkanRenderer3D::BeginSingleTimeCommands()
{
	// Create a single time command buffer trough the command pool manager and return it
	return m_pCommandPoolManager->BeginSingleTimeCommands(D3D::Vulkan3D::GetInstance().GetDevice());
}

void D3D::VulkanRenderer3D::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
{
	// End the single time command buffer trough the commandpool manager
	m_pCommandPoolManager->EndSingleTimeCommands(D3D::Vulkan3D::GetInstance().GetGPUObject(), commandBuffer);
}