// VulkanCore.h
// This class will hold the core Vulkan objects, mainly the instance, physical device and logical device

#ifndef _DDM_VULKAN_CORE_
#define _DDM_VULKAN_CORE_

// File includes
#include "Includes/VulkanIncludes.h"

namespace DDM
{
	class VulkanCore final
	{
	public:
		/// <summary>
		/// Default constructor
		/// </summary>
		VulkanCore();

		/// <summary>
		/// Destructor
		/// </summary>
		~VulkanCore();

		// Delete copy and move constructors
		VulkanCore(VulkanCore&) = delete;
		VulkanCore(VulkanCore&&) = delete;

		// Delete copy and move asignment operator
		VulkanCore& operator=(VulkanCore&) = delete;
		VulkanCore& operator=(VulkanCore&&) = delete;
		

	private:
		// Vulkan instance
		VkInstance m_VkInstance{};

		/// <summary>
		/// Create the vulkan instance
		/// </summary>
		void CreateInstance();

		/// <summary>
		/// Retrieve application info for Instance creation
		/// </summary>
		/// <returns>Application info struct</returns>
		VkApplicationInfo GetApplicationInfo();
	};
}

#endif // !_DDM_VULKAN_CORE_
