// PhysicalDeviceInfo.cpp

// Header include
#include "PhysicalDeviceInfo.h"

// Standard library include
#include <iostream>
#include <set>

DDM::PhysicalDeviceInfo::PhysicalDeviceInfo(VkPhysicalDevice device)
{
	m_VkPhysicalDevice = device;

	SetupFeatures();

	SetupQueueFamilies();

	SetupProperties();
}

DDM::PhysicalDeviceInfo::~PhysicalDeviceInfo()
{
}

const char* const* DDM::PhysicalDeviceInfo::GetRequiredExtensionName() const
{
	return m_RequiredExtensions.data();
}

uint32_t DDM::PhysicalDeviceInfo::GetRequiredExtensionCount() const
{
	return static_cast<uint32_t>(m_RequiredExtensions.size());
}

const VkPhysicalDeviceFeatures& DDM::PhysicalDeviceInfo::GetEnabledFeatures() const
{
	return m_EnabledFeatures;
}

int DDM::PhysicalDeviceInfo::GetScore()
{
	if(!IsDeviceValid())
	{
		return -1;
	}

	constexpr uint32_t baseScore = 1;

	uint32_t score = baseScore;

	// Device type
	// Take a preference of discrete gpu over integrated gpu and prefer both of these over any other type
	constexpr uint32_t integratedMultiplier = 2;
	constexpr uint32_t discreteMultiplier = 3;

	if (m_Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
	{
		score *= integratedMultiplier;
	}
	else if (m_Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
	{
		score *= discreteMultiplier;
	}

	std::cout << "Score of gpu " << m_Properties.deviceName << ": " << score << std::endl;

	return score;
}

void DDM::PhysicalDeviceInfo::FindOptimalQueueFamily(uint32_t& index, uint32_t& count)
{
	index = 0;
	count = 0;

	for (int i{}; i < m_QueueFamilies.size(); ++i)
	{
		if (IsValidQueueFamily(m_QueueFamilies[i]))
		{
			if (m_QueueFamilies[i].queueCount > count)
			{
				count = m_QueueFamilies[i].queueCount;
				index = i;
			}
		}
	}
}

void DDM::PhysicalDeviceInfo::SetupFeatures()
{
	// Enable all available features
	vkGetPhysicalDeviceFeatures(m_VkPhysicalDevice, &m_EnabledFeatures);
}

void DDM::PhysicalDeviceInfo::SetupQueueFamilies()
{
	uint32_t queueFamilyCount{};

	vkGetPhysicalDeviceQueueFamilyProperties(m_VkPhysicalDevice, &queueFamilyCount, nullptr);

	m_QueueFamilies.resize(queueFamilyCount);

	vkGetPhysicalDeviceQueueFamilyProperties(m_VkPhysicalDevice, &queueFamilyCount, m_QueueFamilies.data());
}

void DDM::PhysicalDeviceInfo::SetupProperties()
{
	vkGetPhysicalDeviceProperties(m_VkPhysicalDevice, &m_Properties);
}

bool DDM::PhysicalDeviceInfo::IsDeviceValid()
{
	if (!HasRequiredExtensions())
	{
		return false;
	}

	if (!HasRequiredQueueFamily())
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

bool DDM::PhysicalDeviceInfo::HasRequiredQueueFamily()
{
	for (auto& family : m_QueueFamilies)
	{
		if (IsValidQueueFamily(family))
		{
			return true;
		}
	}

	return false;
}

bool DDM::PhysicalDeviceInfo::IsValidQueueFamily(VkQueueFamilyProperties family)
{
	for (auto bit : m_RequiredQueueFlags)
	{
		if ((family.queueFlags & bit) == 0)
		{
			return false;
		}
	}

	return true;
}