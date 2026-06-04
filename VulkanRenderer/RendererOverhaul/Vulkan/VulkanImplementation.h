// VulkanImplemenation
// This class will hold and manage all vulkan objects

#ifndef _DDM_VULKAN_IMPLEMENTATION_
#define _DDM_VULKAN_IMPLEMENTATION_

// File includes

// Standard library includes
#include <memory>

namespace DDM::Vulkan
{
	// Class forward declarations
	class VulkanAllocator;
	class VulkanCore;
	class VulkanCommandPool;
	class VulkanSwapchain;

	class VulkanImage;
	class VulkanFileImage;

	class VulkanImplementation final
	{
	public:
		/// <summary>
		/// Default constructor
		/// </summary>
		VulkanImplementation();

		/// <summary>
		/// Destructor
		/// </summary>
		~VulkanImplementation();

		// Delete copy and move constructors
		VulkanImplementation(VulkanImplementation&) = delete;
		VulkanImplementation(VulkanImplementation&&) = delete;

		// Delete copy and move asignment operator
		VulkanImplementation& operator=(VulkanImplementation&) = delete;
		VulkanImplementation& operator=(VulkanImplementation&&) = delete;

	private:
		// Custom vulkan allocator
		std::unique_ptr<VulkanAllocator> m_pAllocator{};

		// Core object of vulkan application
		std::unique_ptr<VulkanCore> m_pCore{};

		// Vulkan commandpool used for single commands (transient commandbuffers)
		std::unique_ptr<VulkanCommandPool> m_pTransferCommandPool{};

		// Vulkan swapchain object
		std::unique_ptr<VulkanSwapchain> m_pSwapchain{};

		std::unique_ptr<VulkanFileImage> m_pTestImage{};

		void PresentTestFunction();

		void PipelineTestFunction();
	};
}

#endif // !_DDM_VULKAN_IMPLEMENTATION_
