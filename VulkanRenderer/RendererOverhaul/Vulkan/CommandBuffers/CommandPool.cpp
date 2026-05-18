// CommandPool.cpp

// Header include
#include "CommandPool.h"

// Standard library includes
#include <stdexcept>

// File includes
#include "Vulkan/Core/VulkanAllocator.h"
#include "Vulkan/Core/VulkanCore.h"

DDM::CommandPool::CommandPool(const VulkanAllocator* pAllocator, const VulkanCore* pCore, uint32_t queueFamilyIndex, bool transient, bool reset)
	:
	m_pAllocator{pAllocator},
	m_pCore{pCore},
	m_Transient{ transient },
	m_Reset{ reset }
{
	VkCommandPoolCreateInfo createInfo{};

	SetupCreateInfo(createInfo, queueFamilyIndex);

	if(vkCreateCommandPool(m_pCore->GetDeviceHandle(), &createInfo, m_pAllocator->GetAllocator(), &m_VkCommandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create commandpool!");
	}
}

DDM::CommandPool::~CommandPool()
{
	vkDestroyCommandPool(m_pCore->GetDeviceHandle(), m_VkCommandPool, m_pAllocator->GetAllocator());
}

std::unique_ptr<DDM::CommandBuffer> DDM::CommandPool::GetCommandBuffer() const
{
	return std::make_unique<CommandBuffer>(this);
}

void DDM::CommandPool::SetupCreateInfo(VkCommandPoolCreateInfo& createInfo, uint32_t queueFamilyIndex)
{
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.pNext = nullptr;

	createInfo.flags |= m_Reset ? VK_COMMAND_POOL_CREATE_TRANSIENT_BIT : 0;
	createInfo.flags |= m_Reset ? VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT : 0;

	createInfo.queueFamilyIndex = queueFamilyIndex;
}

void DDM::CommandPool::AllocateCommandBuffer(VkCommandBuffer* pCommandBuffer) const
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

void DDM::CommandPool::FreeCommandBuffer(VkCommandBuffer* pCommandBuffer) const
{
	vkFreeCommandBuffers(m_pCore->GetDeviceHandle(), m_VkCommandPool, 1, pCommandBuffer);
}