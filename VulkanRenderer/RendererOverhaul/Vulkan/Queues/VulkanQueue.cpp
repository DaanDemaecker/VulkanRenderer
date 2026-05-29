// VulkanQueue.cpp

// Header include
#include "VulkanQueue.h"

DDM::Vulkan::VulkanQueue::VulkanQueue(uint32_t familyIndex, uint32_t queueIndex, float priority)
	:m_Priority{priority},
	m_FamilyIndex{familyIndex},
	m_QueueIndex{queueIndex}
{
}

DDM::Vulkan::VulkanQueue::~VulkanQueue()
{

}

void DDM::Vulkan::VulkanQueue::RetrieveQueue(VkDevice device)
{
	vkGetDeviceQueue(device, m_FamilyIndex, m_QueueIndex, &m_VkQueue);
}

void DDM::Vulkan::VulkanQueue::WaitIdle() const
{
	vkQueueWaitIdle(m_VkQueue);
}
