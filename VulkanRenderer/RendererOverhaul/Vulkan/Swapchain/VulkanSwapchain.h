// VulkanSwapchain.h
// A wrapper for a vulkan swapchain

#ifndef _DDM_VULKAN_SWAPCHAIN_
#define _DDM_VULKAN_SWAPCHAIN_

// File includes
#include "Includes/VulkanIncludes.h"

namespace DDM
{
	class VulkanCore;
	class VulkanAllocator;

	class VulkanSwapchain final
	{
	public:
		// Delete default constructor
		VulkanSwapchain() = delete;

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="pAllocator">Pointer to the Vulkan allocator object</param>
		/// <param name="pCore">Pointer to the Vulkan core object</param>
		VulkanSwapchain(const VulkanAllocator* pAllocator, const VulkanCore* pCore);

		/// <summary>
		/// Destructor
		/// </summary>
		~VulkanSwapchain();

		// Delete copy and move operations
		VulkanSwapchain(const VulkanSwapchain&) = delete;
		VulkanSwapchain(VulkanSwapchain&&) = delete;
		VulkanSwapchain& operator=(const VulkanSwapchain&) = delete;
		VulkanSwapchain& operator=(VulkanSwapchain&&) = delete;

	private:
		// Pointer to the Vulkan core
		const VulkanCore* m_pCore;

		// Pointer to the Vulkan allocator
		const VulkanAllocator* m_pAllocator;

		// Swapchain handle
		VkSwapchainKHR m_VkSwapchain{VK_NULL_HANDLE};

		/// <summary>
		/// Create the swapchain object
		/// </summary>
		void CreateSwapchain();
	};
}

#endif // !_DDM_VULKAN_SWAPCHAIN_
