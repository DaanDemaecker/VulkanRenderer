// VulkanImage.h 
// Wrapper for a VKImage class

#ifndef _DDM_VULKAN_IMAGE_
#define _DDM_VULKAN_IMAGE_

// File includes
#include "Includes/VulkanIncludes.h"

// Standard library includes
#include <string>

namespace DDM
{
	// Class forward declaration
	class VulkanCore;
	class VulkanAllocator;
	class VulkanCommandPool;
	class VulkanBuffer;

	class VulkanImage
	{
	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="pAllocator">pointer to the custom allocator</param>
		/// <param name="pCore">pointer to VulkanCore object</param>
		VulkanImage(const VulkanAllocator* pAllocator, const VulkanCore* pCore, const VulkanCommandPool* pCommandPool);

		virtual ~VulkanImage();

		void FillBarrierInfo(VkImageMemoryBarrier& barrier);

		void FillCopyRegionInfo(VkBufferImageCopy& region);

		VkImage GetImage() const { return m_VkImage; }

		VkImageLayout GetLayout() const { return m_VkLayout; }

	protected:
		// Pointer to the custom allocator
		const VulkanAllocator* m_pAllocator;

		// Pointer to the core object
		const VulkanCore* m_pCore;

		// Pointer to the commandpool object to use for barrier transitions and copying
		const VulkanCommandPool* m_pCommandPool;

		// Vulkan image object
		VkImage m_VkImage{};

		// Handle to the vulkan device memory
		VkDeviceMemory m_VkMemory{};

		// The size in pixels of the image
		VkExtent3D m_VkExtent{};

		// Vulkan image layout
		VkImageLayout m_VkLayout{VK_IMAGE_LAYOUT_UNDEFINED};

		// Indicates whether object is correctly initialized
		bool m_Initialized{false};

		// Indicates whether memory was correctly allocated
		bool m_MemoryAllocated{ false };

		/// <summary>
		/// Allocate memory for the image
		/// </summary>
		void AllocateMemory();
	};
}

#endif // !_DDM_VULKAN_IMAGE_
