// VulkanBuffer.cpp

// Header include
#include "VulkanBuffer.h"

// File includes
#include "Vulkan/Core/VulkanCore.h"
#include "Vulkan/Core/VulkanAllocator.h"
#include "Vulkan/Core/PhysicalDeviceInfo.h"

// Standard library includes
#include <stdexcept>

DDM::VulkanBuffer::VulkanBuffer(const VulkanAllocator* pAllocator, const VulkanCore* pCore)
	:m_pAllocator{ pAllocator },
	m_pCore{ pCore }
{

}

DDM::VulkanBuffer::~VulkanBuffer()
{
	if (m_MemoryMapped)
	{
		vkUnmapMemory(m_pCore->GetDeviceHandle(), m_VkMemory);
	}

	if (m_MemoryAllocated)
	{
		vkFreeMemory(m_pCore->GetDeviceHandle(), m_VkMemory, m_pAllocator->GetAllocator());
	}

	if (m_Initialized)
	{
		vkDestroyBuffer(m_pCore->GetDeviceHandle(), m_VkBuffer, m_pAllocator->GetAllocator());
	}
}

void DDM::VulkanBuffer::CreateBuffer(VkDeviceSize size)
{
	m_BufferSize = size;

	VkBufferCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.size = m_BufferSize;
	createInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.queueFamilyIndexCount = 0;
	createInfo.pQueueFamilyIndices = nullptr;

	if (vkCreateBuffer(m_pCore->GetDeviceHandle(), &createInfo, m_pAllocator->GetAllocator(), &m_VkBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create Vulkan buffer");
	}

	m_Initialized = true;

	AllocateMemory();

	MapMemory();

	BindMemory();
}

void DDM::VulkanBuffer::WriteToBuffer(void* data)
{
	if (m_Initialized && m_MemoryAllocated && m_MemoryMapped)
	{
		memcpy(m_MappedMemory, data, m_BufferSize);
	}
}

void DDM::VulkanBuffer::AllocateMemory()
{
	VkMemoryAllocateInfo allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.pNext = nullptr;
	allocateInfo.allocationSize = m_BufferSize;
	allocateInfo.memoryTypeIndex = m_pCore->GetPhysicalDeviceInfo()->GetHostVisibleHeapIndex();

	if (vkAllocateMemory(m_pCore->GetDeviceHandle(), &allocateInfo, m_pAllocator->GetAllocator(), &m_VkMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate Vulkan Device memory");
	}

	m_MemoryAllocated = true;
}

void DDM::VulkanBuffer::MapMemory()
{
	if (vkMapMemory(m_pCore->GetDeviceHandle(), m_VkMemory, 0, m_BufferSize, 0, &m_MappedMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to map Vulkan device memory");
	}

	m_MemoryMapped = true;
}

void DDM::VulkanBuffer::BindMemory()
{
	vkBindBufferMemory(m_pCore->GetDeviceHandle(), m_VkBuffer, m_VkMemory, 0);
}
