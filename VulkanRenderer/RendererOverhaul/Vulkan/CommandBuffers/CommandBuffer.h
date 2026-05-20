// CommandBuffer.h
// This class will serve as a wrapper for a Vulkan CommanBuffer

#ifndef _DDM_COMMAND_BUFFER_
#define _DDM_COMMAND_BUFFER_

// File includes
#include "Includes/VulkanIncludes.h"

namespace DDM
{
	// Class forward declarations
	class VulkanCore;
	class CommandPool;
	class VulkanQueue;
	class VulkanPipelineBarrier;

	class CommandBuffer final
	{
	public:
		CommandBuffer(const CommandPool* pPool, const VulkanQueue* pQueue, const bool resetBitSet);

		~CommandBuffer();

		/// <summary>
		/// Submit the command buffer
		/// </summary>
		void Submit();

		// ------------------------------------------------------------------------------
		// Commands
		// ------------------------------------------------------------------------------

		void CmdPipelineBarrier(VulkanPipelineBarrier* pBarrier);
	private:
		// The owning commandpool
		const CommandPool* m_pPool;

		// The queue to submit to
		const VulkanQueue* m_pQueue;

		// Handle of the vulkan command buffer
		VkCommandBuffer m_VkCommandBuffer{};

		// Indicates whether commandbuffer can be reset
		const bool m_ResetBitSet;
		
		// Indicates whether commandbuffer has been set to record
		bool m_InUse{ false };

		// Indicates whether any commands have been recorded
		bool m_CommandRecorded{ false };

		// Indicates whether queue has been submitted
		bool m_Submitted{ false };

		// Indicates whether queue is unusable
		// This happens when the queue is submitted and the reset bit is not set
		bool m_Unuseable{ false };

		/// <summary>
		/// Make the commandbuffer ready for recording
		/// </summary>
		void BeginCommandBuffer();

		/// <summary>
		/// End recordding of commandbuffer
		/// </summary>
		void EndCommandBuffer();

		/// <summary>
		/// Reset the commandbuffer
		/// </summary>
		void ResetCommandBuffer();

		/// <summary>
		/// Query whether commandbuffer is ready to record
		/// </summary>
		/// <returns>Bool indicating readiness to record</returns>
		bool CanRecord();
	};
}

#endif // !_DDM_COMMAND_BUFFER_
