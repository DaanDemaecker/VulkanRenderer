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
	class STBImage;
	class CommandPool;
	class VulkanBuffer;

	class VulkanImage final
	{
	public:
		VulkanImage(const VulkanAllocator* pAllocator, const VulkanCore* pCore, const CommandPool* pCommandPool);

		~VulkanImage();

		void LoadImage(const std::string& filePath);

		void SetBarrierInfo(VkImageMemoryBarrier& barrier);
	private:
		// Pointer to the custom allocator
		const VulkanAllocator* m_pAllocator;

		// Pointer to the core object
		const VulkanCore* m_pCore;

		// Pointer to the commandpool object
		const CommandPool* m_pCommandPool;

		// Vulkan image object
		VkImage m_VkImage{};

		// Handle to the vulkan device memory
		VkDeviceMemory m_VkMemory{};

		// Vulkan image layout
		VkImageLayout m_VkLayout{VK_IMAGE_LAYOUT_UNDEFINED};

		// Indicates whether object is correctly initialized
		bool m_Initialized{false};

		// Indicates whether memory was correctly allocated
		bool m_MemoryAllocated{ false };

		void CreateImage(STBImage* pSTBImage);

		void AllocateMemory();

		void CopyBufferToImage(VulkanBuffer* pBuffer);
	};
}

#endif // !_DDM_VULKAN_IMAGE_
