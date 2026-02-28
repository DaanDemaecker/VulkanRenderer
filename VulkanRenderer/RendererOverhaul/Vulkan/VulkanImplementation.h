// VulkanImplemenation
// This class will hold and manage all vulkan objects

#ifndef _DDM_VULKAN_IMPLEMENTATION_
#define _DDM_VULKAN_IMPLEMENTATION_

// File includes
#include "Includes/VulkanIncludes.h"

namespace DDM
{
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
		
	};
}

#endif // !_DDM_VULKAN_IMPLEMENTATION_
