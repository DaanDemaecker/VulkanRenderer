// CommandBuffer.cpp

// Header include
#include "CommandBuffer.h"

// File includes
#include "Vulkan/CommandBuffers/CommandPool.h"
#include "Vulkan/Queues/VulkanQueue.h"

// Standard library includes
#include <stdexcept>

DDM::CommandBuffer::CommandBuffer(const CommandPool* pPool, const VulkanQueue* pQueue)
	:m_pPool{pPool},
	m_pQueue{pQueue}
{
	m_pPool->AllocateCommandBuffer(&m_VkCommandBuffer);
}

DDM::CommandBuffer::~CommandBuffer()
{
	m_pPool->FreeCommandBuffer(&m_VkCommandBuffer);
}

void DDM::CommandBuffer::Submit()
{
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = nullptr;
	submitInfo.pWaitDstStageMask = nullptr;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_VkCommandBuffer;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = nullptr;

	if (vkQueueSubmit(m_pQueue->GetQueueHandle(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to submit Vulkan queue");
	}

	m_pQueue->WaitIdle();

	m_Submitted = true;
}

void DDM::CommandBuffer::BeginCommandBuffer()
{
	// Don't begin command buffer if already in use
	if (m_InUse)
	{
		return;
	}

	VkCommandBufferBeginInfo info{};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(m_VkCommandBuffer, &info) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to begin commandbuffer");
	}

	m_InUse = true;
}

void DDM::CommandBuffer::EndCommandBuffer()
{
	// Don't end command buffer if not in use
	if (!m_InUse)
	{
		return;
	}

	if (vkEndCommandBuffer(m_VkCommandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to end commandbuffer");
	}

	m_InUse = false;
}

void DDM::CommandBuffer::ResetCommandBuffer()
{
	// Only reset a commandbuffer after it has been submitted
	if (!m_Submitted)
	{
		return;
	}

	if (vkResetCommandBuffer(m_VkCommandBuffer, 0) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to reset commandbuffer");
	}

	m_InUse = false;
	m_Submitted = false;
}
