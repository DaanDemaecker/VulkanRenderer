// VulkanQueue.h
// A wrapper for a VkQueue object

#ifndef _DDM_VULKAN_QUEUE_
#define _DDM_VULKAN_QUEUE_

// File includes
#include "Includes/VulkanIncludes.h"

namespace DDM
{
	// Class forward declarations

	class VulkanQueue final
	{
	public:
		// Delete default constructor
		VulkanQueue() = delete;

		VulkanQueue(uint32_t familyIndex, uint32_t queueIndex, float priority = 1.0f);

		// Delete copy and move constructors
		VulkanQueue(VulkanQueue&) = delete;
		VulkanQueue(VulkanQueue&&) = delete;

		// Delete copy and move asignment operator
		VulkanQueue& operator=(VulkanQueue&) = delete;
		VulkanQueue& operator=(VulkanQueue&&) = delete;

		// Destructor
		~VulkanQueue();

		/// <summary>
		/// Retrieve the VkQueue object from the device
		/// </summary>
		/// <param name="device">Logical device that owns the queue</param>
		void RetrieveQueue(VkDevice device);

		/// <summary>
		/// Get the queue family index
		/// </summary>
		/// <returns>Index of the queue family</returns>
		uint32_t GetFamilyIndex() const { return m_FamilyIndex; }

		/// <summary>
		/// Get the queue index within the family
		/// </summary>
		/// <returns>Queue index</returns>
		uint32_t GetQueueIndex() const { return m_QueueIndex; }

		/// <summary>
		/// Get the priority of this queue
		/// </summary>
		/// <returns>Value of priority</returns>
		float GetPriority() const { return m_Priority; }

		/// <summary>
		/// Get the handle of the VkQueue object
		/// </summary>
		/// <returns>Handle of queue object</returns>
		VkQueue GetQueueHandle() const { return m_VkQueue; }

		/// <summary>
		/// Wait until the queue finished all tasks
		/// </summary>
		void WaitIdle() const;
	private:
		// Priority of the queue
		const float m_Priority;

		// Index of the vulkan queue family
		const uint32_t m_FamilyIndex;

		// Index of the queue within the family
		const uint32_t m_QueueIndex;

		// Vulkan queue object
		VkQueue m_VkQueue{};
	};
}

#endif // !_DDM_VULKAN_QUEUE_
