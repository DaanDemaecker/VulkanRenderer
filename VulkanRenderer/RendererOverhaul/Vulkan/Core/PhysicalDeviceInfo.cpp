// PhysicalDeviceInfo.cpp

// Header include
#include "PhysicalDeviceInfo.h"

// File includes
#include "Vulkan/Queues/VulkanQueueFamily.h"
#include "Vulkan/Queues/VulkanQueue.h"

// Standard library include
#include <iostream>
#include <set>

DDM::PhysicalDeviceInfo::PhysicalDeviceInfo(VkPhysicalDevice device)
{
	m_VkPhysicalDevice = device;

	SetupFeatures();

	SetupQueueFamilies();

	SetupProperties();

	SetupDeviceMemoryProperties();
}

DDM::PhysicalDeviceInfo::~PhysicalDeviceInfo()
{
}

void DDM::PhysicalDeviceInfo::SetupExtensions(VkDeviceCreateInfo& createInfo)
{
	createInfo.enabledExtensionCount = static_cast<uint32_t>(m_RequiredExtensions.size());
	createInfo.ppEnabledExtensionNames = m_RequiredExtensions.data();
}

const VkPhysicalDeviceFeatures& DDM::PhysicalDeviceInfo::GetEnabledFeatures() const
{
	return m_VkEnabledFeatures;
}

int DDM::PhysicalDeviceInfo::GetScore(const std::vector<uint32_t>& requiredQueueFlags)
{
	if(!IsDeviceValid(requiredQueueFlags))
	{
		return -1;
	}

	constexpr uint32_t baseScore = 1;

	uint32_t score = baseScore;

	// Device type
	// Take a preference of discrete gpu over integrated gpu and prefer both of these over any other type
	constexpr uint32_t integratedMultiplier = 2;
	constexpr uint32_t discreteMultiplier = 3;

	if (m_VkProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
	{
		score *= integratedMultiplier;
	}
	else if (m_VkProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
	{
		score *= discreteMultiplier;
	}

	std::cout << "Score of gpu " << m_VkProperties.deviceName << ": " << score << std::endl;

	return score;
}

bool DDM::PhysicalDeviceInfo::GetQueue(const std::vector<uint32_t>& requiredQueueFlags, uint32_t& familyIndex, uint32_t& queueIndex)
{
	for (const auto& queueFamily : m_QueueFamilies)
	{
		bool validFamily = true;

		for (auto flag : requiredQueueFlags)
		{
			if (!queueFamily->IsFlagSet(flag))
			{
				validFamily = false;
				break;
			}
		}

		if (validFamily)
		{
			familyIndex = queueFamily->GetIndex();
			queueFamily->GetNextQueue(queueIndex);
			return true;
		}
	}

	return false;
}

void DDM::PhysicalDeviceInfo::SetupQueueCreateInfos(std::vector<VulkanQueue*> pQueues, std::vector<VkDeviceQueueCreateInfo>& infos, std::map<uint32_t, std::vector<float>>& priorities)
{
	std::map<uint32_t, uint32_t> queuesPerFamily{};

	for (auto queue : pQueues)
	{
		uint32_t family = queue->GetFamilyIndex();

		uint32_t index = queue->GetQueueIndex();

		if (queuesPerFamily.contains(family))
		{
			queuesPerFamily[family] = std::max(queuesPerFamily[family], index + 1);
		}
		else
		{
			queuesPerFamily[family] = index + 1;
		}

		if (!priorities.contains(family))
		{
			priorities[family] = std::vector<float>();
		}

		if (priorities[family].size() < index + 1)
		{
			priorities[family].resize(index + 1);
		}

		priorities[family][index] = queue->GetPriority();

	}

	for (auto& pair : queuesPerFamily)
	{
		VkDeviceQueueCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = 0;
		info.queueFamilyIndex = pair.first;
		info.queueCount = pair.second;
		info.pQueuePriorities = priorities[pair.first].data();

		infos.push_back(info);
	}
}

void DDM::PhysicalDeviceInfo::SetupFeatures()
{
	// Enable all available features
	vkGetPhysicalDeviceFeatures(m_VkPhysicalDevice, &m_VkEnabledFeatures);
}

void DDM::PhysicalDeviceInfo::SetupQueueFamilies()
{
	uint32_t queueFamilyCount{};

	vkGetPhysicalDeviceQueueFamilyProperties(m_VkPhysicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);

	vkGetPhysicalDeviceQueueFamilyProperties(m_VkPhysicalDevice, &queueFamilyCount, queueFamilies.data());

	m_QueueFamilies.reserve(queueFamilyCount);

	for (uint32_t i{}; i < queueFamilyCount; ++i)
	{
		auto queueFamily = queueFamilies[i];
		m_QueueFamilies.push_back(std::make_shared<VulkanQueueFamily>(queueFamily, i));
	}
}

void DDM::PhysicalDeviceInfo::SetupProperties()
{
	vkGetPhysicalDeviceProperties(m_VkPhysicalDevice, &m_VkProperties);
}

bool DDM::PhysicalDeviceInfo::IsDeviceValid(const std::vector<uint32_t>& requiredQueueFlags)
{
	if (!HasRequiredExtensions())
	{
		return false;
	}

	if (!HasRequiredQueueFamily(requiredQueueFlags))
	{
		return false;
	}

	return true;
}

bool DDM::PhysicalDeviceInfo::HasRequiredExtensions()
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(m_VkPhysicalDevice, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(m_VkPhysicalDevice, nullptr, &extensionCount, availableExtensions.data());

	//Create a set of required extensions to avoid duplicates
	std::set<std::string> requiredExtensions(m_RequiredExtensions.begin(), m_RequiredExtensions.end());

	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

bool DDM::PhysicalDeviceInfo::HasRequiredQueueFamily(const std::vector<uint32_t>& requiredQueueFlags)
{
	std::set<uint32_t> requiredFlagsSet(requiredQueueFlags.begin(), requiredQueueFlags.end());

	for (auto& family : m_QueueFamilies)
	{
		for (auto flag : requiredQueueFlags)
		{
			if (family->IsFlagSet(flag))
			{
				requiredFlagsSet.erase(flag);
				if (requiredFlagsSet.empty())
				{
					return true;
				}
			}
		}
	}

	return false;
}

void DDM::PhysicalDeviceInfo::SetupDeviceMemoryProperties()
{
	vkGetPhysicalDeviceMemoryProperties(m_VkPhysicalDevice, &m_VkMemoryPropeties);

	for (uint32_t index{ 0 }; index < m_VkMemoryPropeties.memoryTypeCount; ++index)
	{
		if ((m_VkMemoryPropeties.memoryTypes[index].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0)
		{
			m_HostVisibleHeapIndex = index;
			break;
		}
	}
}
