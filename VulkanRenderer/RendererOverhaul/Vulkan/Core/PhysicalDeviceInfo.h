// PhysicalDeviceInfo.h
// This class serves as a wrapper for the Vulkan physical device and holds info about the device such as properties and enabled features

#ifndef _DDM_PHYSICAL_DEVICE_INFO_
#define _DDM_PHYSICAL_DEVICE_INFO_

// File includes
#include "Includes/VulkanIncludes.h"

// Standard library includes
#include <vector>	
#include <memory>
#include <map>

namespace DDM
{
	// Class forward declarations
	class VulkanQueueFamily;
	class VulkanQueue;

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
		/// <param name="requiredQueueFlags">A list of the minimum required queue family flags</param>
		/// <returns>Int indicating score, -1 if not valid</returns>
		int GetScore(const std::vector<uint32_t>& requiredQueueFlags);

		/// <summary>
		/// Get the index of the memory heap visible to the host
		/// </summary>
		/// <returns>Index of memory heap</returns>
		uint32_t GetHostVisibleHeapIndex() const { return m_HostVisibleHeapIndex; }

		/// <summary>
		/// Get a queue family index an queue index given a number of specified flags
		/// </summary>
		/// <param name="requiredQueueFlags">Minimum required flags for requested queue</param>
		/// <param name="familyIndex">Will be filled in with index of the queuefamily with requested capabilities</param>
		/// <param name="queueIndex">Will be filled in with index of queue within the family</param>
		/// <returns>Boolean indicating succes</returns>
		bool GetQueue(const std::vector<uint32_t>& requiredQueueFlags, uint32_t& familyIndex, uint32_t& queueIndex);

		/// <summary>
		/// Get a queue family index an queue index that can present given a number of specified flags
		/// </summary>
		/// <param name="requiredQueueFlags">Minimum required flags for requested queue</param>
		/// <param name="surface">Surface to query support for</param>
		/// <param name="familyIndex">Will be filled in with index of the queuefamily with requested capabilities</param>
		/// <param name="queueIndex">Will be filled in with index of queue within the family</param>
		/// <returns>Boolean indicating succes</returns>
		bool GetPresentQueue(const std::vector<uint32_t>& requiredQueueFlags, VkSurfaceKHR surface, uint32_t& familyIndex, uint32_t& queueIndex);

		/// <summary>
		/// Set up a list of VkDeviceQueueCreateInfo structs with the correct values
		/// </summary>
		/// <param name="pQueues">list of all used queues</param>
		/// <param name="infos">list of infos to fill in</param>
		/// <param name="priorities">list of priorities per family index to fill in</param>
		void SetupQueueCreateInfos(std::vector<VulkanQueue*> pQueues, std::vector<VkDeviceQueueCreateInfo>& infos, std::map<uint32_t, std::vector<float>>& priorities);
	
		/// <summary>
		/// Get the memory type with given requirements
		/// </summary>
		/// <param name="requirements">requirements for memory type</param>
		/// <returns>Index of the memory type</returns>
		uint32_t GetMemoryType(const VkMemoryRequirements& requirements) const;
	private:
		// Vulkan physical device
		VkPhysicalDevice m_VkPhysicalDevice{};

		// List of required device extensions
		const std::vector<const char*> m_RequiredExtensions =
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		// Struct of enabled device features
		VkPhysicalDeviceFeatures m_VkEnabledFeatures{};

		// List of queufamily properties
		std::vector<std::shared_ptr<VulkanQueueFamily>> m_QueueFamilies{};

		// Struct holding device properties
		VkPhysicalDeviceProperties m_VkProperties{};

		// Struct holding device memory properties
		VkPhysicalDeviceMemoryProperties m_VkMemoryPropeties{};

		// Index of the memory heap visible to the host
		uint32_t m_HostVisibleHeapIndex{};

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
		bool IsDeviceValid(const std::vector<uint32_t>& requiredQueueFlags);

		/// <summary>
		/// Check if given device has all requested extensions
		/// </summary>
		/// <returns>Bool indicating if device has extensions</returns>
		bool HasRequiredExtensions();

		/// <summary>
		/// Check if given device has queue families required for drawing etc
		/// </summary>
		/// <returns>Bool indicating if device has correct queue families</returns>
		bool HasRequiredQueueFamily(const std::vector<uint32_t>& requiredQueueFlags);

		/// <summary>
		/// Query and set up Vulkan memroy properties
		/// </summary>
		void SetupDeviceMemoryProperties();
	};
}

#endif // !_DDM_PHYSICAL_DEVICE_INFO_