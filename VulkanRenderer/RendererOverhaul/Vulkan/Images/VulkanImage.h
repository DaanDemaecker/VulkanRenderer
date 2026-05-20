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
	private:
		// Pointer to the custom allocator
		const VulkanAllocator* m_pAllocator;

		// Pointer to the core object
		const VulkanCore* m_pCore;

		// Pointer to the commandpool object
		const CommandPool* m_pCommandPool;

		// Vulkan image object
		VkImage m_VkImage{};

		// Indicates whether object is correctly initialized
		bool m_Initialized{false};

		void CreateImage(STBImage* pSTBImage);

		void CopyBufferToImage(VulkanBuffer* pBuffer);
	};
}

#endif // !_DDM_VULKAN_IMAGE_
