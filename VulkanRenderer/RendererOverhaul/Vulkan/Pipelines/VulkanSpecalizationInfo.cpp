// VulkanSpecalizationInfo.cpp

// Header include
#include "VulkanSpecalizationInfo.h"

// Standard library includes
#include <iostream>

void DDM::Vulkan::VulkanSpecalizationInfo::SetEntry(uint32_t id, size_t size, void* pData)
{
	// Set the current entry index to the size of the entries vector
	// This will be used to check if the entry already exists
	uint32_t currentEntryIndex{static_cast<uint32_t>(m_Entries.size())};

	// Check if entry with this id already exists, if so, set the current entry index to that entry's index
	for (uint32_t i{}; i < m_Entries.size(); ++i)
	{
		if (m_Entries[i].id == id)
		{
			currentEntryIndex = i;
			break;
		}
	}

	// If no entry with this id exists, create a new entry and set the current entry index to that entry's index
	if (currentEntryIndex == m_Entries.size())
	{
		m_Entries.push_back(VulkanSpecInfoEntry());
	}

	// Retrieve a reference to the current entry and set its id and data
	auto& currentEntry = m_Entries[currentEntryIndex];

	currentEntry.id = id;
	currentEntry.data = std::vector<char>(size);
	memcpy(currentEntry.data.data(), pData, size);
}

void DDM::Vulkan::VulkanSpecalizationInfo::FillSpecInfo(VkSpecializationInfo& specInfo)
{
	CalculateFinalData();

	specInfo.dataSize = static_cast<uint32_t>(m_FinalData.size());
	specInfo.pData = m_FinalData.data();

	specInfo.mapEntryCount = static_cast<uint32_t>(m_VkEntries.size());
	specInfo.pMapEntries = m_VkEntries.data();
}

void DDM::Vulkan::VulkanSpecalizationInfo::CalculateFinalData()
{
	// Calculate the total size of the data to be sent to the shader
	uint32_t sizeInBytes{};

	for (auto& entry : m_Entries)
	{
		sizeInBytes += static_cast<uint32_t>(entry.data.size());
	}

	// Resize the final data vector to the total size
	m_FinalData.resize(sizeInBytes);


	// Copy the data from each entry into the final data vector
	uint32_t offset{};
	for (auto& entry : m_Entries)
	{
		memcpy(m_FinalData.data() + offset, entry.data.data(), entry.data.size());
		offset += static_cast<uint32_t>(entry.data.size());
	}


	// Resize vkentries to the same size as the entries vector
	m_VkEntries.resize(m_Entries.size());

	offset = 0;

	// Fill in the vkentries with the data from the entries vector
	for (uint32_t i{}; i < m_Entries.size(); ++i)
	{
		m_VkEntries[i].constantID = m_Entries[i].id;
		m_VkEntries[i].size = m_Entries[i].data.size();
		m_VkEntries[i].offset = offset;

		offset += static_cast<uint32_t>(m_Entries[i].data.size());
	}
}