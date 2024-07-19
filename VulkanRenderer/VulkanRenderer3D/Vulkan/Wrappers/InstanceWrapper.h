// InstanceWrapper.h
// This class will serve as a wrapper for the VkInstance object

#ifndef InstanceWrapperIncluded
#define InstanceWrapperIncluded

// File includes
#include "Includes/VulkanIncludes.h"

// Standard library includes
#include <vector>
#include <iostream>

namespace D3D
{
	class InstanceWrapper final
	{
	public:
		// Constructor
		InstanceWrapper();

		// Destructor
		~InstanceWrapper();

		// Delete copy and move functions
		InstanceWrapper(InstanceWrapper& other) = delete;
		InstanceWrapper(InstanceWrapper&& other) = delete;
		InstanceWrapper& operator=(InstanceWrapper& other) = delete;
		InstanceWrapper& operator=(InstanceWrapper&& other) = delete;

		// Get the VkInstance
		VkInstance GetInstance() const { return m_Instance; }

		// Are validation layers enabled
		bool ValidationLayersEnabled() const { return m_EnableValidationLayers; }

		// Get validation layers
		const std::vector<const char*>& GetValidationLayers() const { return m_ValidationLayers; }

	private:
		// Handle of the VkInstance
		VkInstance m_Instance = VK_NULL_HANDLE;

		// Handle for the debug messenger
		VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;

		// Should validation layers be enabled
		bool m_EnableValidationLayers{ true };

		// The requested validation layers
		const std::vector<const char*> m_ValidationLayers =
		{
			"VK_LAYER_KHRONOS_validation"
		};

		// Set the values for the application info
		// Parameters:
		//     applicationInfo: a reference to the VkApplicationInfo struct to avoid making a new one in the function
		//     applicationName: a reference to an empty string to avoid the application name going out of scope
		//     engineName: a reference to an empty string to avoid the engine name going out of scope
		void SetupApplicationInfo(VkApplicationInfo& appInfo, std::string& applicationName, std::string& engineName);

		// Helper function that checks if given validation layers are supported
		// Parameters:
		//     validationLayers: list of validation layers that should be checked
		bool CheckValidationLayerSupport(const std::vector<const char*> validationLayers);

		// Get the required extensions for the instance createion
		// Parameters:
		//     enableValidationLayers: are the validation layers enabled
		std::vector<const char*> GetRequiredExtensions(bool enableValidationLayers);

		// Setup the debug messenger create info
		// Parameters:
		//     createInfo: a reference to the messenger create info to avoid creating a new one in the function
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		// Necesarry function for the callback of the debug messenger
		// Parameters:
		//     messageSeverity: the severity of the current message
		//     messageType: the type of the current message
		//     pCallbackData: the pointer that contains the message
		//     pUserData: data of current user
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData
		);

		// Set up the debug messenger
		// Parameters:
		//     enableValidationLayers: are the validation layers enabled
		void SetupDebugMessenger(bool enableValidationLayers);

		// Create the debug messenger
		// Parameters:
		//     instance: handle of the VKInstance
		//     pCreateInfo: pointer to the messenger create info
		//     pAllocator: pointer for callbacks
		//     pDebugMessenger: pointer to the debug messenger
		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pDebugMessenger);
	
		// Destroy the debug messenger
		// Parameters:
		//     instance: handle of the VKInstance
		//     pDebugMessenger: pointer to the debug messenger
		//     pAllocator: pointer for callbacks
		void DestroyDebugUtilsMessegerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
			const VkAllocationCallbacks* pAllocator);
	};

}
#endif // !InstanceWrapperIncluded