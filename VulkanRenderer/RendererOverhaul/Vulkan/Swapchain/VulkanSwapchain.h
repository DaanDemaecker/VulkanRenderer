// VulkanSwapchain.h
// A wrapper for a vulkan swapchain

#ifndef _DDM_VULKAN_SWAPCHAIN_
#define _DDM_VULKAN_SWAPCHAIN_

// File includes
#include "Includes/VulkanIncludes.h"
#include "Vulkan/Images/ImageTypes/VulkanSwapchainImage.h"

// Standard library includes
#include <memory>
#include <vector>

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

		// Swapchain images
		std::vector<std::unique_ptr<VulkanSwapchainImage>> m_SwapchainImages;

		/// <summary>
		/// Create the swapchain object
		/// </summary>
		void CreateSwapchain();

		/// <summary>
		/// Retrieve the VKImage objects from the swapchain
		/// </summary>
		void RetrieveImages();
	};
}

#endif // !_DDM_VULKAN_SWAPCHAIN_
