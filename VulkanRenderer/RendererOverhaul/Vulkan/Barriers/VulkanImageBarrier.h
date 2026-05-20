// VulkanIMageBarier.h
// A wrapper for a VkImageMemoryBarrier struct

#ifndef _DDM_VULKAN_IMAGE_BARRIER_
#define _DDM_VULKAN_IMAGE_BARRIER_

// File includes
#include "Includes/VulkanIncludes.h"

namespace DDM
{
	// Class forward declarations
	class VulkanImage;

	class VulkanImageBarrier
	{
	public:
		VulkanImageBarrier() = delete;

		VulkanImageBarrier(VulkanImage* pImage, VkImageLayout newLayout);

		~VulkanImageBarrier();

		// Delete copy and move constructors
		VulkanImageBarrier(VulkanImageBarrier&) = delete;
		VulkanImageBarrier(VulkanImageBarrier&&) = delete;

		// Delete copy and move asignment operators
		VulkanImageBarrier& operator=(VulkanImageBarrier&) = delete;
		VulkanImageBarrier& operator=(VulkanImageBarrier&&) = delete;

		/// <summary>
		/// Return the handle of the vulkan image memory barrier struct
		/// </summary>
		/// <returns>Struct by value</returns>
		VkImageMemoryBarrier GetHandle() const { return m_VkImageMemoryBarrier; }
	private:
		// Image barrier struct
		VkImageMemoryBarrier m_VkImageMemoryBarrier{};

	};
}

#endif // !_DDM_VULKAN_IMAGE_BARRIER_
