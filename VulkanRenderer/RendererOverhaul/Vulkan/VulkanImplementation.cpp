// VulkanImplementation.cpp

// Header include
#include "VulkanImplementation.h"

// File includes
#include "Vulkan/Core/VulkanAllocator.h"
#include "Vulkan/Core/VulkanCore.h"

#include "Vulkan/CommandBuffers/CommandPool.h"
#include "Vulkan/CommandBuffers/CommandBuffer.h"

DDM::VulkanImplementation::VulkanImplementation()
{
	m_pAllocator = std::make_unique<VulkanAllocator>();

	m_pCore = std::make_unique<VulkanCore>(m_pAllocator.get());

	m_pShortTermCommandPool = std::make_unique<CommandPool>(m_pAllocator.get(), m_pCore.get(), m_pCore->GetPrimaryQueueFamily(), true, false);
}

DDM::VulkanImplementation::~VulkanImplementation()
{

}
