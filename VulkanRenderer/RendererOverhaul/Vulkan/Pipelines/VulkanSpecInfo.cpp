// VulkanSpecInfo.cpp

// Header include
#include "VulkanSpecInfo.h"

// Standard library includes
#include <iostream>

void DDM::Vulkan::VulkanSpecInfo::SetEntry(uint32_t id, size_t size, void* pData)
{
	uint32_t currentEntryIndex{static_cast<uint32_t>(m_Entries.size())};

	for (uint32_t i{}; i < m_Entries.size(); ++i)
	{
		if (m_Entries[i].id == id)
		{
			currentEntryIndex = i;
			break;
		}
	}

	if (currentEntryIndex == m_Entries.size())
	{
		m_Entries.push_back(VulkanSpecInfoEntry());
	}

	auto& currentEntry = m_Entries[currentEntryIndex];

	currentEntry.id = id;
	currentEntry.data = std::vector<char>(size);
	memcpy(currentEntry.data.data(), pData, size);
}

void DDM::Vulkan::VulkanSpecInfo::FillSpecInfo(VkSpecializationInfo& specInfo)
{
	CalculateFinalData();

	specInfo.dataSize = static_cast<uint32_t>(m_FinalData.size());
	specInfo.pData = m_FinalData.data();

	specInfo.mapEntryCount = static_cast<uint32_t>(m_VkEntries.size());
	specInfo.pMapEntries = m_VkEntries.data();
}

void DDM::Vulkan::VulkanSpecInfo::CalculateFinalData()
{
	uint32_t sizeInBytes{};

	for (auto& entry : m_Entries)
	{
		sizeInBytes += static_cast<uint32_t>(entry.data.size());
	}

	m_FinalData.resize(sizeInBytes);

	uint32_t offset{};

	for (auto& entry : m_Entries)
	{
		memcpy(m_FinalData.data() + offset, entry.data.data(), entry.data.size());
		offset += static_cast<uint32_t>(entry.data.size());
	}


	m_VkEntries.resize(m_Entries.size());

	offset = 0;

	for (uint32_t i{}; i < m_Entries.size(); ++i)
	{
		m_VkEntries[i].constantID = m_Entries[i].id;
		m_VkEntries[i].size = m_Entries[i].data.size();
		m_VkEntries[i].offset = offset;

		offset += static_cast<uint32_t>(m_Entries[i].data.size());
	}
}