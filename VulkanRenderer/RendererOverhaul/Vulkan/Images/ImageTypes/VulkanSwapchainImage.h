// VulkanSwapchainImage.h
// A wrapper for a VKImage class that is retrieved from the swapchain

#ifndef _DDM_VULKAN_SWAPCHAIN_IMAGE_
#define _DDM_VULKAN_SWAPCHAIN_IMAGE_

// Parent include
#include "Vulkan/Images/VulkanImage.h"

// Standard library includes
#include <memory>

namespace DDM::Vulkan
{
	class VulkanSwapchainImage final : public VulkanImage
	{
	public:
		friend class VulkanSwapchain;

		// Delete default constructor
		VulkanSwapchainImage() = delete;

		/// <summary>
		/// Destructor
		/// </summary>
		virtual ~VulkanSwapchainImage();

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="pAllocator">pointer to the custom allocator</param>
		/// <param name="pCore">pointer to VulkanCore object</param>
		VulkanSwapchainImage(const VulkanAllocator* pAllocator, const VulkanCore* pCore, VkImage image);
	private:
		static std::unique_ptr<VulkanSwapchainImage> Create(const VulkanAllocator* pAllocator, const VulkanCore* pCore, VkImage image);

	};
}



#endif // !_DDM_VULKAN_SWAPCHAIN_IMAGE_