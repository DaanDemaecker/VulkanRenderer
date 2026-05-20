// CommandPool.h
// This class will serve as a wrapper for a VkCommandPool object

#ifndef _DDM_COMMAND_POOL_
#define _DDM_COMMAND_POOL_

// File includes
#include "Includes/VulkanIncludes.h"
#include "Vulkan/CommandBuffers/CommandBuffer.h"

// Standard library includes
#include <memory>

namespace DDM
{
	// Class forward declarations
	class VulkanAllocator;
	class VulkanCore;
	class VulkanQueue;

	class CommandPool final
	{
	public:
		// Delete default constructor
		CommandPool() = delete;

		// Delete copy and move constructors
		CommandPool(CommandPool&) = delete;
		CommandPool(CommandPool&&) = delete;

		// Delete copy and move assignment operators
		CommandPool& operator=(CommandPool&) = delete;
		CommandPool& operator=(CommandPool&&) = delete;
		
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="pAllocator">pointer to the custom allocator</param>
		/// <param name="pCore">pointer to VulkanCore object</param>
		/// <param name="pQueue">pointer to the VkQueue wrapper</param>
		/// <param name="transient">set to true if commandbuffers are for short duration</param>
		/// <param name="reset">set to true if commandbuffers should be reused</param>
		CommandPool(const VulkanAllocator* pAllocator, const VulkanCore* pCore, const VulkanQueue* pQueue, bool transient, bool reset);

		/// <summary>
		/// Destructor
		/// </summary>
		~CommandPool();

		/// <summary>
		/// Create and retrieve a new commandbuffer
		/// </summary>
		/// <returns></returns>
		std::unique_ptr<CommandBuffer> GetCommandBuffer() const;
	private:
		friend class CommandBuffer;

		// Pointer to the custom allocator
		const VulkanAllocator* m_pAllocator;

		// Pointer to the core object
		const VulkanCore* m_pCore;

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

#endif // !_DDM_COMMAND_POOL_
