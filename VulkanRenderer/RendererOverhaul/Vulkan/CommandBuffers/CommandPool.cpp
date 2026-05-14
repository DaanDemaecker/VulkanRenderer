// CommandPool.cpp

// Header include
#include "CommandPool.h"

// File includes
#include "Vulkan/Core/VulkanAllocator.h"

DDM::CommandPool::CommandPool(VulkanAllocator* pAllocator, VkDevice device, uint32_t queueFamilyIndex, bool transient, bool reset)
	:
	m_pAllocator{pAllocator},
	m_VkDevice{device},
	m_Transient{ transient },
	m_Reset{ reset }
{
	VkCommandPoolCreateInfo createInfo{};

	SetupCreateInfo(createInfo, queueFamilyIndex);

	vkCreateCommandPool(m_VkDevice, &createInfo, m_pAllocator->GetAllocator(), &m_VkCommandPool);
}

DDM::CommandPool::~CommandPool()
{
	vkDestroyCommandPool(m_VkDevice, m_VkCommandPool, m_pAllocator->GetAllocator());
}

void DDM::CommandPool::SetupCreateInfo(VkCommandPoolCreateInfo& createInfo, uint32_t queueFamilyIndex)
{
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.pNext = nullptr;

	createInfo.flags |= m_Reset ? VK_COMMAND_POOL_CREATE_TRANSIENT_BIT : 0;
	createInfo.flags |= m_Reset ? VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT : 0;

	createInfo.queueFamilyIndex = queueFamilyIndex;
}
