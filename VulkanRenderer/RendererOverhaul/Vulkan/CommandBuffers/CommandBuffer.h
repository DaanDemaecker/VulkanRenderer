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

	class CommandBuffer final
	{
	public:
		CommandBuffer(const CommandPool* pPool, const VulkanQueue* pQueue);

		~CommandBuffer();
		void Submit();


		void PipelineBarrier();
	private:
		// The owning commandpool
		const CommandPool* m_pPool;

		// The queue to submit to
		const VulkanQueue* m_pQueue;

		// Handle of the vulkan command buffer
		VkCommandBuffer m_VkCommandBuffer{};
		
		// Indicates whether commandbuffer has been set to record
		bool m_InUse{ false };

		// Indicates whether any commands have been recorded
		bool m_CommandRecorder{ false };

		// Indicates whether queue has been submitted
		bool m_Submitted{ false };

		void SetupCommandBuffer();

		void BeginCommandBuffer();

		void EndCommandBuffer();

		void ResetCommandBuffer();
	};
}

#endif // !_DDM_COMMAND_BUFFER_
