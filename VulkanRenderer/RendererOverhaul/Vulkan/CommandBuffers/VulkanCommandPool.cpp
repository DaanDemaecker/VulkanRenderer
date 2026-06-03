// CommandPool.cpp

// Header include
#include "VulkanCommandPool.h"

// Standard library includes
#include <stdexcept>

// File includes
#include "Vulkan/Core/VulkanAllocator.h"
#include "Vulkan/Core/VulkanCore.h"
#include "Vulkan/Queues/VulkanQueue.h"

DDM::Vulkan::VulkanCommandPool::VulkanCommandPool(const VulkanAllocator* pAllocator, const VulkanCore* pCore, const VulkanQueue* pQueue, bool transient, bool reset)
	:VulkanObject(pCore, pAllocator),
	m_pQueue{pQueue},
	m_TransientBitSet{ transient },
	m_ResetBitSet{ reset }
{
	VkCommandPoolCreateInfo createInfo{};

	SetupCreateInfo(createInfo);

	if(vkCreateCommandPool(m_pCore->GetDeviceHandle(), &createInfo, m_pAllocator->GetAllocator(), &m_VkCommandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create commandpool!");
	}
}

DDM::Vulkan::VulkanCommandPool::~VulkanCommandPool()
{
	vkDestroyCommandPool(m_pCore->GetDeviceHandle(), m_VkCommandPool, m_pAllocator->GetAllocator());
}

std::unique_ptr<DDM::Vulkan::VulkanCommandBuffer> DDM::Vulkan::VulkanCommandPool::GetCommandBuffer() const
{
	return std::make_unique<VulkanCommandBuffer>(this, m_pQueue, m_ResetBitSet);
}

void DDM::Vulkan::VulkanCommandPool::SetupCreateInfo(VkCommandPoolCreateInfo& createInfo)
{
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.pNext = nullptr;

	createInfo.flags |= m_TransientBitSet ? VK_COMMAND_POOL_CREATE_TRANSIENT_BIT : 0;
	createInfo.flags |= m_ResetBitSet ? VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT : 0;

	createInfo.queueFamilyIndex = m_pQueue->GetFamilyIndex();
}

void DDM::Vulkan::VulkanCommandPool::AllocateCommandBuffer(VkCommandBuffer* pCommandBuffer) const
{
	VkCommandBufferAllocateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	info.pNext = nullptr;
	info.commandPool = m_VkCommandPool;
	info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	info.commandBufferCount = 1;


	if (vkAllocateCommandBuffers(m_pCore->GetDeviceHandle(), &info, pCommandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate command buffer");
	}
}

void DDM::Vulkan::VulkanCommandPool::FreeCommandBuffer(VkCommandBuffer* pCommandBuffer) const
{
	vkFreeCommandBuffers(m_pCore->GetDeviceHandle(), m_VkCommandPool, 1, pCommandBuffer);
}