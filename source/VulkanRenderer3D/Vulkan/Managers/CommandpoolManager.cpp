// CommandpoolManager.cpp

// File includes
#include "CommandpoolManager.h"
#include "../VulkanUtils.h"
#include "../Wrappers/GPUObject.h"

// Standard library includes
#include <stdexcept>

D3D::CommandpoolManager::CommandpoolManager(GPUObject* pGPUObject, VkSurfaceKHR surface, uint32_t frames)
{
	// Initialize the commandpool
	CreateCommandPool(pGPUObject, surface);
	// Initialize the commandbuffers
	CreateCommandBuffers(pGPUObject->GetDevice(), frames);
}

void D3D::CommandpoolManager::Cleanup(VkDevice device)
{
	// Destroy the commandpool
	vkDestroyCommandPool(device, m_CommandPool, nullptr);
}

void D3D::CommandpoolManager::CreateCommandPool(GPUObject* pGPUObject, VkSurfaceKHR surface)
{
	// Get the needed queuefamilies
	D3D::QueueFamilyIndices queueFamilyIndices = VulkanUtils::FindQueueFamilies(pGPUObject->GetPhysicalDevice(), surface);

	// Create commandpool create info object
	VkCommandPoolCreateInfo poolInfo{};
	// Set type to command pool create info
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	// Set flag to command pool create reset command buffer
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	// Give the needed graphics family
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	// Create the commandpool
	if (vkCreateCommandPool(pGPUObject->GetDevice(), &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to create command pool!");
	}
}

void D3D::CommandpoolManager::CreateCommandBuffers(VkDevice device, uint32_t frames)
{
	// Resize the commandbuffer vector to the amount of frames
	m_CommandBuffers.resize(frames);

	// Create command buffer allocate info
	VkCommandBufferAllocateInfo allocInfo{};
	// Set type to command buffer allocate info
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	// Set commandpool
	allocInfo.commandPool = m_CommandPool;
	// Set level to pripary
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	// Set commandbuffer count to max amount of frames
	allocInfo.commandBufferCount = frames;

	// Allocate the commandbuffers
	if (vkAllocateCommandBuffers(device, &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

VkCommandBuffer D3D::CommandpoolManager::BeginSingleTimeCommands(VkDevice device)
{
	// Create commandbuffer allocate info
	VkCommandBufferAllocateInfo allocInfo{};
	// Set type to command buffer allocate info
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	// Set command buffer level to primary
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	// Set commandpool
	allocInfo.commandPool = m_CommandPool;
	// Set buffercount to 1
	allocInfo.commandBufferCount = 1;

	// Create commandbuffer handle
	VkCommandBuffer commandBuffer;
	// Allocate command buffer
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	// Create commandbuffer begin info
	VkCommandBufferBeginInfo beginInfo{};
	// Set type to command buffer begin info
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	// Set flags to usage one time submit
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	// Begin the command buffer
	vkBeginCommandBuffer(commandBuffer, &beginInfo);
	
	// Return the command buffer
	return commandBuffer;
}

void D3D::CommandpoolManager::EndSingleTimeCommands(GPUObject* pGPUObject, VkCommandBuffer commandBuffer)
{
	// End the command buffer
	vkEndCommandBuffer(commandBuffer);

	// Create submit info
	VkSubmitInfo submitInfo{};
	// Set type to submit info
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	// Set command buffer count to 1
	submitInfo.commandBufferCount = 1;
	// Give pointer to commandbuffer
	submitInfo.pCommandBuffers = &commandBuffer;

	// Get graphics queue
	auto graphicsQueue{ pGPUObject->GetQueueObject().graphicsQueue };

	// Submit graphics queue
	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	// Wait until graphics queue is done
	vkQueueWaitIdle(graphicsQueue);

	// Free the command buffers
	vkFreeCommandBuffers(pGPUObject->GetDevice(), m_CommandPool, 1, &commandBuffer);
}