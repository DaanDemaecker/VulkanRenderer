// CommandpoolManager.cpp

// File includes
#include "CommandpoolManager.h"
#include "Vulkan/VulkanUtils.h"
#include "Vulkan/Wrappers/GPUObject.h"
#include "Vulkan/Vulkan3D.h"

// Standard library includes
#include <stdexcept>

DDM3::CommandpoolManager::CommandpoolManager(GPUObject* pGPUObject, VkSurfaceKHR surface)
{
	// Initialize the commandpool
	CreateCommandPool(pGPUObject, surface);
	// Initialize the commandbuffers
	CreateCommandBuffers(pGPUObject->GetDevice());
}

DDM3::CommandpoolManager::~CommandpoolManager()
{
	Cleanup(Vulkan3D::GetInstance().GetDevice());
}

void DDM3::CommandpoolManager::Cleanup(VkDevice device)
{
	// Destroy the commandpool
	vkDestroyCommandPool(device, m_CommandPool, nullptr);
}

void DDM3::CommandpoolManager::CreateCommandPool(GPUObject* pGPUObject, VkSurfaceKHR surface)
{
	// Get the needed queuefamilies
	DDM3::QueueFamilyIndices queueFamilyIndices = VulkanUtils::FindQueueFamilies(pGPUObject->GetPhysicalDevice(), surface);

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

void DDM3::CommandpoolManager::CreateCommandBuffers(VkDevice device)
{
	auto frames = Vulkan3D::GetMaxFrames();
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

VkCommandBuffer DDM3::CommandpoolManager::BeginSingleTimeCommands(VkDevice device)
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

void DDM3::CommandpoolManager::EndSingleTimeCommands(GPUObject* pGPUObject, VkCommandBuffer commandBuffer)
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