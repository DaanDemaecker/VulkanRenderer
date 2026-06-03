// CommandPool.h
// This class will serve as a wrapper for a VkCommandPool object

#ifndef _DDM_VULKAN_COMMAND_POOL_
#define _DDM_VULKAN_COMMAND_POOL_

// Parent include
#include "Vulkan/Core/VulkanObject.h"

// File includes
#include "Includes/VulkanIncludes.h"
#include "Vulkan/CommandBuffers/VulkanCommandBuffer.h"

// Standard library includes
#include <memory>

namespace DDM::Vulkan
{
	// Class forward declarations
	class VulkanQueue;

	class VulkanCommandPool final : public VulkanObject
	{
	public:
		// Delete default constructor
		VulkanCommandPool() = delete;

		// Delete copy and move constructors
		VulkanCommandPool(VulkanCommandPool&) = delete;
		VulkanCommandPool(VulkanCommandPool&&) = delete;

		// Delete copy and move assignment operators
		VulkanCommandPool& operator=(VulkanCommandPool&) = delete;
		VulkanCommandPool& operator=(VulkanCommandPool&&) = delete;
		
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="pAllocator">pointer to the custom allocator</param>
		/// <param name="pCore">pointer to VulkanCore object</param>
		/// <param name="pQueue">pointer to the VkQueue wrapper</param>
		/// <param name="transient">set to true if commandbuffers are for short duration</param>
		/// <param name="reset">set to true if commandbuffers should be reused</param>
		VulkanCommandPool(const VulkanAllocator* pAllocator, const VulkanCore* pCore, const VulkanQueue* pQueue, bool transient, bool reset);

		/// <summary>
		/// Destructor
		/// </summary>
		~VulkanCommandPool();

		/// <summary>
		/// Create and retrieve a new commandbuffer
		/// </summary>
		/// <returns></returns>
		std::unique_ptr<VulkanCommandBuffer> GetCommandBuffer() const;
	private:
		friend class VulkanCommandBuffer;

		// Pointer to the queue object
		const VulkanQueue* m_pQueue;

		// Handle of the commandpool
		VkCommandPool m_VkCommandPool{};

		// Indicates whether transient bit is set
		const bool m_TransientBitSet;

		// Indicates whether reset bit is set
		const bool m_ResetBitSet;

		/// <summary>
		/// Set up the createinfo struct for creation of commandpool
		/// </summary>
		/// <param name="createInfo:>reference to the creatinfo struct to fill in</param>
		void SetupCreateInfo(VkCommandPoolCreateInfo& createInfo);

		/// <summary>
		/// Allocate a new command buffer
		/// </summary>
		/// <param name="pCommandBuffer">pointer to a handle to fill in with new command buffer</param>
		void AllocateCommandBuffer(VkCommandBuffer* pCommandBuffer) const;

		/// <summary>
		/// Free a given commandbuffer
		/// </summary>
		/// <param name="pCommandBuffer">Commandbuffer to free</param>
		void FreeCommandBuffer(VkCommandBuffer* pCommandBuffer) const;
	};
}

#endif // !_DDM_VULKAN_COMMAND_POOL_
