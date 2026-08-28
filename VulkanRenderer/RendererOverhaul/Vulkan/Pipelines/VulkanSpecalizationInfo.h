// VulkanSpecalizationInfo.h
// This class will serve as a helper to initialize specialization info during pipeline creations

#ifndef _DDM_VULKAN_SPECALIZATION_INFO_
#define _DDM_VULKAN_SPECALIZATION_INFO_

// File includes
#include "Includes/VulkanIncludes.h"

// Standard library includes
#include <vector>

namespace DDM::Vulkan
{
	class VulkanSpecalizationInfo final
	{
	public:
		// Default constructor
		VulkanSpecalizationInfo() = default;

		// Default destructor
		~VulkanSpecalizationInfo() = default;

		/// <summary>
		/// Add a single entry to specify in the shader
		/// </summary>
		/// <param name="id">Id of the constant in the shader</param>
		/// <param name="size">Size of the data</param>
		/// <param name="pData">Pointer to the raw data</param>
		void SetEntry(uint32_t id, size_t size, void* pData);

		/// <summary>
		/// Fill in the specialization info with the given entries
		/// </summary>
		/// <param name="specInfo">Specialization info to fill in</param>
		void FillSpecInfo(VkSpecializationInfo& specInfo);

	private:
		struct VulkanSpecInfoEntry
		{
			std::vector<char> data= std::vector<char>(); // Raw data to send to the shader
			uint32_t id = static_cast<uint32_t>(-1); // Shader ID of the entry
		};

		// List of entries
		std::vector<VulkanSpecInfoEntry> m_Entries{};

		// Final glob of data to send to the shader
		std::vector<char> m_FinalData{};

		// List of entries of the vulkan structs
		std::vector<VkSpecializationMapEntry> m_VkEntries{};

		/// <summary>
		/// Calculate the final data vector and the vulkan entries
		/// </summary>
		void CalculateFinalData();
	};
}

#endif // !_DDM_VULKAN_SPECALIZATION_INFO_
