// CommandBuffer.cpp

// Header include
#include "VulkanCommandBuffer.h"

// File includes
#include "Vulkan/CommandBuffers/VulkanCommandPool.h"
#include "Vulkan/Queues/VulkanQueue.h"
#include "Vulkan/Barriers/VulkanPipelineBarrier.h"
#include "Vulkan/Images/VulkanImage.h"
#include "Vulkan/Buffers/VulkanBuffer.h"

// Standard library includes
#include <stdexcept>
#include <iostream>

DDM::VulkanCommandBuffer::VulkanCommandBuffer(const VulkanCommandPool* pPool, const VulkanQueue* pQueue, const bool resetBitSet)
	:m_pPool{pPool},
	m_pQueue{pQueue},
	m_ResetBitSet{resetBitSet}
{
	m_pPool->AllocateCommandBuffer(&m_VkCommandBuffer);
}

DDM::VulkanCommandBuffer::~VulkanCommandBuffer()
{
	m_pPool->FreeCommandBuffer(&m_VkCommandBuffer);
}

void DDM::VulkanCommandBuffer::Submit()
{
	if (m_InUse)
	{
		EndCommandBuffer();
	}

	if (!m_CommandRecorded)
	{
		return;
	}

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

	if (!m_ResetBitSet)
	{
		m_Unuseable = true;
	}
}


// ------------------------------------------------------------------------------
// Commands
// ------------------------------------------------------------------------------


void DDM::VulkanCommandBuffer::CmdPipelineBarrier(VulkanPipelineBarrier* pBarrier)
{
	// Check if commandbuffer can record commands
	if (!CanRecord())
	{
		return;
	}

	pBarrier->Execute(m_VkCommandBuffer);

	m_CommandRecorded = true;
}

void DDM::VulkanCommandBuffer::CmdCopyBufferToImage(VulkanImage* pImage, VulkanBuffer* pBuffer)
{
	// Check if commandbuffer can record commands
	if (!CanRecord())
	{
		return;
	}

	VkBufferImageCopy region{};

	pImage->FillCopyRegionInfo(region);

	pBuffer->FillCopyRegionInfo(region);

	CmdCopyBufferToImage(pImage, pBuffer, region);
}

void DDM::VulkanCommandBuffer::CmdCopyBufferToImage(VulkanImage* pImage, VulkanBuffer* pBuffer, VkBufferImageCopy& region)
{
	// Check if commandbuffer can record commands
	if (!CanRecord())
	{
		return;
	}

	vkCmdCopyBufferToImage(m_VkCommandBuffer, pBuffer->GetBuffer(), pImage->GetImage(), pImage->GetLayout(), 1, &region);

	m_CommandRecorded = true;
}

// ------------------------------------------------------------------------------
// Internal functions
// ------------------------------------------------------------------------------

void DDM::VulkanCommandBuffer::BeginCommandBuffer()
{
	// Don't begin command buffer if already in use or commandbuffer is unuseable
	if (m_InUse || m_Unuseable)
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

void DDM::VulkanCommandBuffer::EndCommandBuffer()
{
	// Don't end command buffer if not in use or if commandbuffer is unuseable
	if (!m_InUse || m_Unuseable)
	{
		return;
	}

	if (vkEndCommandBuffer(m_VkCommandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to end commandbuffer");
	}

	m_InUse = false;
}

void DDM::VulkanCommandBuffer::ResetCommandBuffer()
{
	// Only reset a commandbuffer after it has been submitted, it is useable and the reset bit has been set
	if (!m_ResetBitSet && !m_Submitted)
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

bool DDM::VulkanCommandBuffer::CanRecord()
{
	if (m_Unuseable)
	{
		std::cout << "Command buffer is not useable \n";
		return false;
	}

	if (m_Submitted && m_ResetBitSet)
	{
		ResetCommandBuffer();
	}

	if (!m_InUse)
	{
		BeginCommandBuffer();
	}

	return true;
}
