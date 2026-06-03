// VulkanBuffer.h
// A wrapper for a VkBuffer object

#ifndef _DDM_VULKAN_BUFFER_
#define _DDM_VULKAN_BUFFER_

// Parent include
#include "Vulkan/Core/VulkanObject.h"

// File includes
#include "Includes/VulkanIncludes.h"

namespace DDM::Vulkan
{
	class VulkanBuffer final : public VulkanObject
	{
	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="pAllocator">pointer to the custom allocator</param>
		/// <param name="pCore">pointer to VulkanCore object</param>
		VulkanBuffer(const VulkanAllocator* pAllocator, const VulkanCore* pCore);

		// Destructor
		virtual ~VulkanBuffer();

		// Delete default constructor
		VulkanBuffer() = delete;

		// Delete copy and move constructors
		VulkanBuffer(VulkanBuffer&) = delete;
		VulkanBuffer(VulkanBuffer&&) = delete;

		// Delete copy and move assignment operators
		VulkanBuffer& operator=(VulkanBuffer&) = delete;
		VulkanBuffer& operator=(VulkanBuffer&&) = delete;

		/// <summary>
		/// Create the buffer object
		/// </summary>
		/// <param name="size">Size of the buffer in size</param>
		void CreateBuffer(VkDeviceSize size);

		/// <summary>
		/// Write data to the buffer
		/// </summary>
		/// <param name="data">Pointer to the data to write</param>
		void WriteToBuffer(void* data);

		/// <summary>
		/// Enter the region info for a copy command
		/// </summary>
		/// <param name="region">Reference to the struct to fill in</param>
		void FillCopyRegionInfo(VkBufferImageCopy& region);

		/// <summary>
		/// Get the handle of the vulkan buffer
		/// </summary>
		/// <returns>Buffer handle</returns>
		VkBuffer GetBuffer() const { return m_VkBuffer; }
	private:
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
