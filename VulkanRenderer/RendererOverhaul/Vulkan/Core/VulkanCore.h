// VulkanCore.h
// This class will hold the core Vulkan objects, mainly the instance, physical device and logical device

#ifndef _DDM_VULKAN_CORE_
#define _DDM_VULKAN_CORE_

// File includes
#include "Includes/VulkanIncludes.h"

// Standard library includes
#include <vector>

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
		// ---------------------------------------
		// Instance
		//----------------------------------------

		// Vulkan instance
		VkInstance m_VkInstance{};

		// Debug messenger
		VkDebugUtilsMessengerEXT m_VkDebugMessenger{};

		// List of names of validation layers
		const std::vector<char const*> m_ValidationLayers = {
			"VK_LAYER_KHRONOS_validation"
			};

		// Indicates whether validation layers should be active
		bool m_EnableValidationLayers = true;

		// ---------------------------------------
		// Physical device
		//----------------------------------------

		// Vulkan physical device
		VkPhysicalDevice m_VkPhysicalDevice{};


		// ---------------------------------------
		// Instance
		//----------------------------------------

		/// <summary>
		/// Create the vulkan instance
		/// </summary>
		void CreateInstance();

		/// <summary>
		/// Retrieve application info for Instance creation
		/// </summary>
		/// <returns>Application info struct</returns>
		VkApplicationInfo GetApplicationInfo();

		/// <summary>
		/// Set up the validation layers for the instance creation info struct
		/// </summary>
		/// <param name="createInfo: ">reference to the VkInstanceCreateInfo struct</param>
		void SetupValidationLayers(VkInstanceCreateInfo& createInfo);

		/// <summary>
		/// Check if validation layers are supported
		/// </summary>
		/// <returns>bool indicating support for validation layers</returns>
		bool QueryValidationLayerSupport();

		/// <summary>
		/// Get the required extensions for the instance
		/// </summary>
		/// <returns>List of required extensions</returns>
		std::vector<const char*> GetExtensions();

		/// <summary>
		/// Initialize the debug messenger
		/// </summary>
		void SetupDebugMessenger();

		/// <summary>
		/// Fill in all fields for the debug messenger create info
		/// </summary>
		/// <param name="createInfo: ">Reference to the createinfo to fill in</param>
		void PopulateDebugMessenger(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		/// <summary>
		/// Set up required severities for the debug messenger
		/// </summary>
		/// <param name="createInfo: ">reference to the create info struct for debug messenger</param>
		void SetupDebugMessengerSeverities(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		/// <summary>
		/// Set up required message types for the debug messenger
		/// </summary>
		/// <param name="createInfo: ">reference to the create info struct for debug messenger</param>
		void SetupDebugMessengerTypes(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		/// <summary>
		/// Create the debug messenger
		/// </summary>
		/// <param name="instance: ">Vulkan instance associated with messenger</param>
		/// <param name="pCreateInfo: ">Create info of the messenger</param>
		/// <param name="pAllocator: ">Custom vulkan allocator</param>
		/// <param name="pDebugMessenger: ">Pointer to the future debug messenger</param>
		/// <returns>Result of the operation</returns>
		VkResult CreateDebugMessenger(VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pDebugMessenger);

		/// <summary>
		/// Destroy the debug messenger
		/// </summary>
		/// <param name="instance: ">Vulkan instance associated with messenger</param>
		/// <param name="debugMessenger: ">Messenger to destroy</param>
		/// <param name="pAllocator: ">Custom vulkan allocator</param>
		void DestroyDebugMessenger(VkInstance instance,
			VkDebugUtilsMessengerEXT debugMessenger,
			const VkAllocationCallbacks* pAllocator);

		/// <summary>
		/// Debug messenger callback
		/// </summary>
		/// <param name="messageSeverity: ">Severity of the message</param>
		/// <param name="messageType: ">Type of message</param>
		/// <param name="pCallbackData: ">Data about the debug callback</param>
		/// <param name="pUserData: ">Data specified by user</param>
		/// <returns></returns>
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);


		// ---------------------------------------
		// Physical device
		//----------------------------------------

		/// <summary>
		/// Do all the setup necessary for the vulkan physical device
		/// </summary>
		void SetupPhysicalDevice();

		/// <summary>
		/// Pick the physical device to use for rendering
		/// </summary>
		/// <returns>Handle of the physical device</returns>
		VkPhysicalDevice PickPhysicalDevice();

		/// <summary>
		/// Give a score to the current physical device to compare to others
		/// </summary>
		/// <param name="physicalDevice: ">device to score</param>
		/// <returns>Score of the device</returns>
		uint32_t ScorePhysicalDevice(VkPhysicalDevice physicalDevice);
	};
}

#endif // !_DDM_VULKAN_CORE_
