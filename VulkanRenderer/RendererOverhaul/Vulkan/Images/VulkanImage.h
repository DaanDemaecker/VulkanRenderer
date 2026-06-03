// VulkanImage.h 
// Wrapper for a VKImage class

#ifndef _DDM_VULKAN_IMAGE_
#define _DDM_VULKAN_IMAGE_

// Parent include
#include "Vulkan/Core/VulkanObject.h"

// File includes
#include "Includes/VulkanIncludes.h"

// Standard library includes
#include <string>

namespace DDM::Vulkan
{
	// Class forward declaration
	class VulkanCommandPool;
	class VulkanBuffer;

	class VulkanImage : public VulkanObject
	{
	public:
		// Delete default constructor
		VulkanImage() = delete;

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="pAllocator">pointer to the custom allocator</param>
		/// <param name="pCore">pointer to VulkanCore object</param>
		VulkanImage(const VulkanAllocator* pAllocator, const VulkanCore* pCore, const VulkanCommandPool* pCommandPool);

		virtual ~VulkanImage();

		void FillBarrierInfo(VkImageMemoryBarrier& barrier);

		void FillCopyRegionInfo(VkBufferImageCopy& region);

		void FillImageToImageSourceRegionInfo(VkImageCopy& region);

		void FIllImageToImageDestinationRegionInfo(VkImageCopy& region);

		VkImage GetImage() const { return m_VkImage; }

		VkImageLayout GetLayout() const { return m_VkLayout; }

		void SetLayout(VkImageLayout newLayout) { m_VkLayout = newLayout; }

	protected:
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
