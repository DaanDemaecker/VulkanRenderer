// CommandPool.h
// This class will serve as a wrapper for a VkCommandPool object

#ifndef _DDM_COMMAND_POOL_
#define _DDM_COMMAND_POOL_

// File includes
#include "Includes/VulkanIncludes.h"

namespace DDM
{
	// Class forward declarations
	class VulkanAllocator;

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
		/// <param name="device">handle of the Vulkan device</param>
		/// <param name="queueFamilyIndex">index of the queuefamily to use for this pool</param>
		/// <param name="transient">set to true if commandbuffers are for short duration</param>
		/// <param name="reset">set to true if commandbuffers should be reused</param>
		CommandPool(VulkanAllocator* pAllocator, VkDevice device, uint32_t queueFamilyIndex, bool transient, bool reset);

		/// <summary>
		/// Destructor
		/// </summary>
		~CommandPool();

	private:
		// Pointer to the custom allocator
		VulkanAllocator* m_pAllocator{};

		// Handle of the owning device
		VkDevice m_VkDevice{};

		// Handle of the commandpool
		VkCommandPool m_VkCommandPool{};

		// Indicates whether transient bit is set
		bool m_Transient;

		// Indicates whether reset bit is set
		bool m_Reset;

		/// <summary>
		/// Set up the createinfo struct for creation of commandpool
		/// </summary>
		/// <param name="createInfo:>reference to the creatinfo struct to fill in</param>
		/// <param name="queueFamilyIndex">index of the queuefamily to use for this pool</param>
		void SetupCreateInfo(VkCommandPoolCreateInfo& createInfo, uint32_t queueFamilyIndex);
	};
}

#endif // !_DDM_COMMAND_POOL_
