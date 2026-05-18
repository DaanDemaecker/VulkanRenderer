// VulkanBuffer.h
// A wrapper for a VkBuffer object

#ifndef _DDM_VULKAN_BUFFER_
#define _DDM_VULKAN_BUFFER_

// File includes
#include "Includes/VulkanIncludes.h"

namespace DDM
{
	// Class forward declarations
	class VulkanAllocator;
	class VulkanCore;

	class VulkanBuffer final
	{
	public:
		VulkanBuffer(const VulkanAllocator* pAllocator, const VulkanCore* pCore);

		~VulkanBuffer();


		void CreateBuffer(VkDeviceSize size);

		void WriteToBuffer(void* data);

	private:
		// Pointer to the custom allocator
		const VulkanAllocator* m_pAllocator;

		// Pointer to the core object
		const VulkanCore* m_pCore;

		// Vulkan buffer object
		VkBuffer m_VkBuffer{};

		// The size in bytes of the buffer
		VkDeviceSize m_BufferSize{};

		// Vulkan memory object
		VkDeviceMemory m_VkMemory{};

		// Pointer to the device memory
		void* m_MappedMemory{};

		// Indicates whether object is correctly initialized
		bool m_Initialized{ false };

		// Indicates wheteher device memory is allocated
		bool m_MemoryAllocated{ false };

		// Indicates whether buffer is mapped to device memory
		bool m_MemoryMapped{ false };

		/// <summary>
		/// Allocate memory for the buffer
		/// </summary>
		void AllocateMemory();

		/// <summary>
		/// Map the memory of the VkMemory object
		/// </summary>
		void MapMemory();

		/// <summary>
		/// Bind the buffer to the device memroy
		/// </summary>
		void BindMemory();
	};
}

#endif // !_DDM_VULKAN_BUFFER_
