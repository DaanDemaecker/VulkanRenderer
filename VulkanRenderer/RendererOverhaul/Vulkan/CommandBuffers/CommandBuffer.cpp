// CommandBuffer.cpp

// Header include
#include "CommandBuffer.h"

// File includes
#include "Vulkan/CommandBuffers/CommandPool.h"

DDM::CommandBuffer::CommandBuffer(const CommandPool* pPool)
	:m_pPool{pPool}
{
	m_pPool->AllocateCommandBuffer(&m_VkCommandBuffer);
}

DDM::CommandBuffer::~CommandBuffer()
{
	m_pPool->FreeCommandBuffer(&m_VkCommandBuffer);
}
