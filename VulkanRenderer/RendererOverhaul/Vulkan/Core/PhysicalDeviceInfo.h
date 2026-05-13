// PhysicalDeviceInfo.h
// This class serves as a wrapper for the Vulkan physical device and holds info about the device such as properties and enabled features

#ifndef _DDM_PHYSICAL_DEVICE_INFO_
#define _DDM_PHYSICAL_DEVICE_INFO_

// File includes
#include "Includes/VulkanIncludes.h"

// Standard library includes
#include <vector>	
#include <memory>

namespace DDM
{
	class PhysicalDeviceInfo final
	{
	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="device">physical device to wrap</param>
		PhysicalDeviceInfo(VkPhysicalDevice device);

		// Delete default constructor
		PhysicalDeviceInfo() = delete;

		/// <summary>
		/// Destructor
		/// </summary>
		~PhysicalDeviceInfo();

		// Delete copy and move operations
		PhysicalDeviceInfo(const PhysicalDeviceInfo&) = delete;
		PhysicalDeviceInfo(PhysicalDeviceInfo&&) = delete;

		PhysicalDeviceInfo& operator=(const PhysicalDeviceInfo&) = delete;
		PhysicalDeviceInfo& operator=(PhysicalDeviceInfo&&) = delete;

		/// <summary>
		/// Get the handle of the vulkan physical device
		/// </summary>
		/// <returns>Handle of the vulkan physical device</returns>
		VkPhysicalDevice GetDeviceHandle() const { return m_VkPhysicalDevice; }

		/// <summary>
		/// Set up the extensions in the VkDeviceCreateInfo struct
		/// </summary>
		/// <param name="createInfo: ">reference to the VkDeviceCreateInfo struct</param>
		void SetupExtensions(VkDeviceCreateInfo& createInfo);

		/// <summary>
		/// Get a list of enabled features
		/// </summary>
		/// <returns>Const ref to a struct that holds enabled features</returns>
		const VkPhysicalDeviceFeatures& GetEnabledFeatures() const;

		/// <summary>
		/// Get the score given to this specific device
		/// </summary>
		/// <returns>Int indicating score, -1 if not valid</returns>
		int GetScore();


		/// <summary>
		/// Find the queuefamily that is most optimal
		/// </summary>
		/// <param name="index">will be filled in with index of the family</param>
		/// <param name="count">will be filled in with the max amount of queues in the family</param>
		void FindOptimalQueueFamily(uint32_t& index, uint32_t& count);

	private:
		// Vulkan physical device
		VkPhysicalDevice m_VkPhysicalDevice{};

		// List of required device extensions
		const std::vector<const char*> m_RequiredExtensions =
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		// List of required flags for queue families
		const std::vector<uint32_t> m_RequiredQueueFlags =
		{
			VK_QUEUE_GRAPHICS_BIT,
			VK_QUEUE_TRANSFER_BIT
		};

		// Struct of enabled device features
		VkPhysicalDeviceFeatures m_VkEnabledFeatures{};

		// List of queufamily properties
		std::vector<VkQueueFamilyProperties> m_VkQueueFamilies{};

		// Struct holding device properties
		VkPhysicalDeviceProperties m_VkProperties{};

		/// <summary>
		/// Fetch and set up all features to enable
		/// </summary>
		void SetupFeatures();

		/// <summary>
		/// Fetch all queue families
		/// </summary>
		void SetupQueueFamilies();

		/// <summary>
		/// Fetch the properties of the device
		/// </summary>
		void SetupProperties();

		/// <summary>
		/// Check if the given device is a valid option
		/// </summary>
		/// <returns>Bool indicating validity</returns>
		bool IsDeviceValid();

		/// <summary>
		/// Check if given device has all requested extensions
		/// </summary>
		/// <returns>Bool indicating if device has extensions</returns>
		bool HasRequiredExtensions();

		/// <summary>
		/// Check if given device has queue families required for drawing etc
		/// </summary>
		/// <returns>Bool indicating if device has correct queue families</returns>
		bool HasRequiredQueueFamily();

		/// <summary>
		/// Check if given queue family has required flags
		/// </summary>
		/// <param name="family">queue family to check</param>
		/// <returns>Bool indicating if family is valid</returns>
		bool IsValidQueueFamily(VkQueueFamilyProperties family);
	};
}

#endif // !_DDM_PHYSICAL_DEVICE_INFO_