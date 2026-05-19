// VulkanQueueFamily.h
// A wrapper for VkQueueFamilyProperties objects

#ifndef _DDM_VULKAN_QUEUE_FAMILY_
#define _DDM_VULKAN_QUEUE_FAMILY_

// File includes
#include "Includes/VulkanIncludes.h"

namespace DDM
{
	class VulkanQueueFamily final
	{
	public:
		// Delete default constructor
		VulkanQueueFamily() = delete;

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="queueFamilyProperties">Queue to wrap</param>
		/// <param name="index">Queue family</param>
		VulkanQueueFamily(const VkQueueFamilyProperties& queueFamilyProperties, const uint32_t index);

		/// <summary>
		/// Query whether queue family has certain flag set
		/// </summary>
		/// <param name="requestedFlag">Flag to query</param>
		/// <returns>Bool indicating if family has flag</returns>
		bool IsFlagSet(uint32_t requestedFlag);
		
		/// <summary>
		/// Get the index of the next available queue
		/// </summary>
		/// <param name="queueIndex">Will be filled in with the queue index</param>
		/// <returns>Bool indicating success</returns>
		bool GetNextQueue(uint32_t& queueIndex);

		bool IsActive() const { return m_IsActive; }

		bool AmountUsed() const { return m_CurrentQueue; }

		uint32_t GetIndex() { return m_Index; }
	private:
		const uint32_t m_Index;

		const VkQueueFamilyProperties m_VkQueueFamilyProperties;

		uint32_t m_CurrentQueue{};

		bool m_IsActive{ false };
	};
}

#endif // !_DDM_VULKAN_QUEUE_FAMILY_
