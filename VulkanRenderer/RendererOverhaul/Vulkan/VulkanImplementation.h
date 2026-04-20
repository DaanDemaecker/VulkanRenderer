// VulkanImplemenation
// This class will hold and manage all vulkan objects

#ifndef _DDM_VULKAN_IMPLEMENTATION_
#define _DDM_VULKAN_IMPLEMENTATION_

// File includes

// Standard library includes
#include <memory>

namespace DDM
{
	class VulkanAllocator;
	class VulkanCore;

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
	};
}

#endif // !_DDM_VULKAN_IMPLEMENTATION_
