// VulkanRenderer.cpp

// File includes
#include "VulkanRenderer3D.h"
#include "D3DEngine.h"
#include "Utils.h"
#include "Model.h"
#include "VulkanUtils.h"
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
#include "GPUObject.h"
#include "BufferManager.h"
#include "PipelineWrapper.h"

#include "SkyBox.h"

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
	m_pGpuObject->WaitIdle();

	// Clean up ImGui
	CleanupImGui();

	// Clean up vulkan objects
	CleanupVulkan();
}

void D3D::VulkanRenderer3D::SetupSkybox()
{
	// Set the vertex shader name
	const std::string vertShaderName{ "../Resources/Shaders/Skybox.Vert.spv" };
	// Set the fragment shader name
	const std::string fragShaderName{ "../Resources/Shaders/Skybox.Frag.spv" };

	// Create the graphics pipeline for the skybox
	AddGraphicsPipeline("Skybox", { vertShaderName, fragShaderName }, false);

	// Create the skybox
	m_pSkyBox = std::make_unique<SkyBox>(
		std::initializer_list<const std::string>{"../resources/images/CubeMap/Sky_Right.png",
												"../resources/images/CubeMap/Sky_Left.png",
												"../resources/images/CubeMap/Sky_Up.png", 
												"../resources/images/CubeMap/Sky_Down.png", 
												"../resources/images/CubeMap/Sky_Front.png", 
												"../resources/images/CubeMap/Sky_Back.png"});

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
	m_pGlobalLight->SetDirection(glm::vec3{ -.577, -.577f, .577 });
	// Set color of global light
	m_pGlobalLight->SetColor(glm::vec3{ 1.f, 1.f, 1.f });
	// Set intensity of global light
	m_pGlobalLight->SetIntensity(1.f);
}

void D3D::VulkanRenderer3D::SetupDefaultPipeline()
{
	// Add the default pipeline
	m_pPipelineManager->AddDefaultPipeline(m_pGpuObject->GetDevice(), m_pRenderpassWrapper->GetRenderpass(), m_pSwapchainWrapper->GetMsaaSamples());
}

void D3D::VulkanRenderer3D::CleanupLight()
{
	m_pGlobalLight = nullptr;
}

void D3D::VulkanRenderer3D::CleanupVulkan()
{
	// Create buffer manager
	m_pBufferManager = std::make_unique<BufferManager>();

	// Get handle to logical device
	auto device{ m_pGpuObject->GetDevice() };

	// Clean up global light
	m_pGlobalLight->Cleanup(device);

	// Clean up swapchain
	m_pSwapchainWrapper->Cleanup(device);

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

	// Clean up GPUObject
	m_pGpuObject->CleanUp();

	// Destroy the surface
	vkDestroySurfaceKHR(m_pInstanceWrapper->GetInstance(), m_Surface, nullptr);
}

void D3D::VulkanRenderer3D::CleanupImGui()
{
	// Clean up ImGui
	m_pImGuiWrapper->Cleanup(m_pGpuObject->GetDevice());
}

void D3D::VulkanRenderer3D::InitVulkan()
{
	// Initialize the vulkan instance
	m_pInstanceWrapper = std::make_unique<InstanceWrapper>();

	// Initialize the surface
	CreateSurface();

	// Initialize the gpu object
	m_pGpuObject = std::make_unique<GPUObject>(m_pInstanceWrapper.get(), m_Surface);

	// Get pointer to gpu object
	GPUObject* pGPUObject{ m_pGpuObject.get() };

	// Initialize command pool manager
	m_pCommandPoolManager = std::make_unique<CommandpoolManager>(pGPUObject, m_Surface, m_MaxFramesInFlight);

	// Initialize the image manager
	m_pImageManager = std::make_unique<ImageManager>(pGPUObject, m_pBufferManager.get(), m_pCommandPoolManager.get());

	// Get the max amount of samples per pixel
	auto msaaSamples = VulkanUtils::GetMaxUsableSampleCount(m_pGpuObject->GetPhysicalDevice());

	// Initialize the swapchain
	m_pSwapchainWrapper = std::make_unique<SwapchainWrapper>(pGPUObject, m_Surface,	m_pImageManager.get(), msaaSamples);

	// Initialize the renderpass
	m_pRenderpassWrapper = std::make_unique<RenderpassWrapper>(pGPUObject->GetDevice(), m_pSwapchainWrapper->GetFormat(), VulkanUtils::FindDepthFormat(pGPUObject->GetPhysicalDevice()), msaaSamples);

	// Create a single time command buffer
	auto commandBuffer{ m_pCommandPoolManager->BeginSingleTimeCommands(pGPUObject->GetDevice())};
	// Initialize swapchain
	m_pSwapchainWrapper->SetupImageViews(pGPUObject, m_pImageManager.get(),commandBuffer, m_pRenderpassWrapper->GetRenderpass());
	// End the single time command buffer
	m_pCommandPoolManager->EndSingleTimeCommands(pGPUObject, commandBuffer);

	// Initialize graphics pipeline manager
	m_pPipelineManager = std::make_unique<PipelineManager>();

	// Initialize the sync objects
	m_pSyncObjectManager = std::make_unique<SyncObjectManager>(pGPUObject->GetDevice(), m_MaxFramesInFlight);
}

void D3D::VulkanRenderer3D::InitImGui()
{
	// Create ImGui vulkan init info
	ImGui_ImplVulkan_InitInfo init_info = {};
	// Give the vulkan instance
	init_info.Instance = m_pInstanceWrapper->GetInstance();
	// Give the physical device
	init_info.PhysicalDevice = m_pGpuObject->GetPhysicalDevice();
	// Give the logical device
	init_info.Device = m_pGpuObject->GetDevice();
	// Give the index of the graphics queue family
	init_info.QueueFamily = m_pGpuObject->GetQueueObject().graphicsQueueIndex;
	// Give the graphics queue
	init_info.Queue = m_pGpuObject->GetQueueObject().graphicsQueue;
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
	m_pImGuiWrapper = std::make_unique<D3D::ImGuiWrapper>(init_info, m_pRenderpassWrapper->GetRenderpass(), commandBuffer, m_pGpuObject->GetDevice(), m_MaxFramesInFlight);
	// End the single time command buffer
	EndSingleTimeCommands(commandBuffer);
}

void D3D::VulkanRenderer3D::AddGraphicsPipeline(const std::string& pipelineName, std::initializer_list<const std::string>&& filePaths, bool hasDepthStencil)
{
	// Add a graphics pipeline trough the pipeline manager
	m_pPipelineManager->AddGraphicsPipeline(m_pGpuObject->GetDevice(), m_pRenderpassWrapper->GetRenderpass(),
		m_pSwapchainWrapper->GetMsaaSamples(), pipelineName, filePaths, hasDepthStencil);
}

void D3D::VulkanRenderer3D::Render(std::vector<std::unique_ptr<Model>>& pModels)
{
	// Wait for the in flight fence of the current frame
	vkWaitForFences(m_pGpuObject->GetDevice(), 1, &m_pSyncObjectManager->GetInFlightFence(m_CurrentFrame), VK_TRUE, UINT64_MAX);

	// Create image index uint
	uint32_t imageIndex{};
	// Get the index of the next image
	VkResult result = vkAcquireNextImageKHR(m_pGpuObject->GetDevice(), m_pSwapchainWrapper->GetSwapchain(), UINT64_MAX, m_pSyncObjectManager->GetImageAvailableSemaphore(m_CurrentFrame), VK_NULL_HANDLE, &imageIndex);

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
	vkResetFences(m_pGpuObject->GetDevice(), 1, &m_pSyncObjectManager->GetInFlightFence(m_CurrentFrame));

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
	if (vkQueueSubmit(m_pGpuObject->GetQueueObject().graphicsQueue, 1, &submitInfo, m_pSyncObjectManager->GetInFlightFence(m_CurrentFrame)) != VK_SUCCESS)
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
	result = vkQueuePresentKHR(m_pGpuObject->GetQueueObject().presentQueue, &presentInfo);

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

	// Render the skybox
	m_pSkyBox->Render();

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

void D3D::VulkanRenderer3D::Render(Model* pModel, VkCommandBuffer& commandBuffer, const VkDescriptorSet* descriptorSet, const PipelineWrapper* pipeline)
{
	// Bind the correct pipeline
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());

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
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipelineLayout(), 0, 1, descriptorSet, 0, nullptr);

	// Draw the current model
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(pModel->GetIndexAmount()), 1, 0, 0, 0);
}

VkDevice D3D::VulkanRenderer3D::GetDevice()
{
	return m_pGpuObject->GetDevice();
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
	return m_pCommandPoolManager->GetCommandBuffer(m_CurrentFrame);
}

const D3D::DirectionalLightStruct& D3D::VulkanRenderer3D::GetGlobalLight() const
{
	// Get a reference to the global light struct
	return m_pGlobalLight->GetLight();
}

D3D::DescriptorObject* D3D::VulkanRenderer3D::GetLightDescriptor()
{
	// Return buffers of the global light object
	return m_pGlobalLight->GetDescriptorObject();
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
	vkDeviceWaitIdle(m_pGpuObject->GetDevice());
	
	// Create a single time command
	auto commandBuffer{ BeginSingleTimeCommands() };

	// Recreate the swapchain
	m_pSwapchainWrapper->RecreateSwapChain(m_pGpuObject.get(), m_Surface, m_pImageManager.get(),
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
	m_pBufferManager->CreateBuffer(m_pGpuObject.get(), size, usage, properties, buffer, bufferMemory);
}

void D3D::VulkanRenderer3D::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	m_pBufferManager->CopyBuffer(m_pGpuObject.get(), m_pCommandPoolManager.get(), srcBuffer, dstBuffer, size);
}

void D3D::VulkanRenderer3D::CreateVertexBuffer(std::vector<D3D::Vertex>& vertices, VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory)
{
	m_pBufferManager->CreateVertexBuffer(m_pGpuObject.get(), m_pCommandPoolManager.get(), vertices, vertexBuffer, vertexBufferMemory);
}

void D3D::VulkanRenderer3D::CreateIndexBuffer(std::vector<uint32_t>& indices, VkBuffer& indexBuffer, VkDeviceMemory& indexBufferMemory)
{
	m_pBufferManager->CreateIndexBuffer(m_pGpuObject.get(), m_pCommandPoolManager.get(), indices, indexBuffer, indexBufferMemory);
}

void D3D::VulkanRenderer3D::UpdateUniformBuffer(UniformBufferObject& buffer)
{
	// Update the buffer with the camera transformation
	m_pCamera->UpdateUniformBuffer(buffer, m_pSwapchainWrapper->GetExtent());
}

void D3D::VulkanRenderer3D::CreateTexture(Texture& texture, const std::string& textureName)
{
	// Create the image trough the image manager
	m_pImageManager->CreateTextureImage(m_pGpuObject.get(), m_pBufferManager.get(), texture, textureName, m_pCommandPoolManager.get());
	// Create the image view
	texture.imageView = m_pImageManager->CreateImageView(m_pGpuObject->GetDevice(), texture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, texture.mipLevels);
}

void D3D::VulkanRenderer3D::CreateCubeTexture(Texture& cubeTexture, const std::initializer_list<const std::string>& textureNames, uint32_t& miplevels)
{
	m_pImageManager->CreateCubeTexture(m_pGpuObject.get(), m_pBufferManager.get(), cubeTexture, textureNames, miplevels, m_pCommandPoolManager.get());
}

VkCommandBuffer D3D::VulkanRenderer3D::BeginSingleTimeCommands()
{
	// Create a single time command buffer trough the command pool manager and return it
	return m_pCommandPoolManager->BeginSingleTimeCommands(m_pGpuObject->GetDevice());
}

void D3D::VulkanRenderer3D::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
{
	// End the single time command buffer trough the commandpool manager
	m_pCommandPoolManager->EndSingleTimeCommands(m_pGpuObject.get(), commandBuffer);
}