#include "VulkanQueueFamily.h"

DDM::Vulkan::VulkanQueueFamily::VulkanQueueFamily(const VkQueueFamilyProperties& queueFamilyProperties, const uint32_t index)
	:m_VkQueueFamilyProperties{queueFamilyProperties},
	m_Index{index}
{

}

bool DDM::Vulkan::VulkanQueueFamily::IsFlagSet(uint32_t requestedFlag)
{
	return (m_VkQueueFamilyProperties.queueFlags & requestedFlag) != 0;
}

bool DDM::Vulkan::VulkanQueueFamily::GetNextQueue(uint32_t& queueIndex)
{
	if (queueIndex >= m_VkQueueFamilyProperties.queueCount)
	{
		return false;
	}

	queueIndex = m_CurrentQueue;

	++m_CurrentQueue;

	return true;
}
