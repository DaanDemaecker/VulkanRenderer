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

	class CommandBuffer final
	{
	public:
		CommandBuffer(const CommandPool* pPool);

		~CommandBuffer();

		void BeginCommandBuffer();

		void EndCommandBuffer();

		void ResetCommandBuffer();
	private:
		// The owning commandpool
		const CommandPool* m_pPool;

		// Handle of the vulkan command buffer
		VkCommandBuffer m_VkCommandBuffer{};
		
		bool m_InUse{ false };

		void SetupCommandBuffer();
	};
}

#endif // !_DDM_COMMAND_BUFFER_
