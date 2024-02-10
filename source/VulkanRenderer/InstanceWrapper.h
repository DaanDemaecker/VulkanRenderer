// InstanceWrapper.h
// This class will serve as a wrapper for the VkInstance object

#ifndef InstanceWrapperDefined
#define InstanceWrapperDefined

#include "GLFWIncludes.h"

#include <vector>

namespace D3D
{

	class InstanceWrapper
	{
	public:
		InstanceWrapper() = delete;

		InstanceWrapper(bool enableValidationLayers, const std::vector<const char*>& validationLayers);

		~InstanceWrapper() = default;

		InstanceWrapper(InstanceWrapper& other) = delete;
		InstanceWrapper(InstanceWrapper&& other) = delete;

		InstanceWrapper& operator=(InstanceWrapper& other) = delete;
		InstanceWrapper& operator=(InstanceWrapper&& other) = delete;

		VkInstance GetInstance() const { return m_Instance; }

		void cleanup(bool enableValidationLayers);

	private:
		VkInstance m_Instance = VK_NULL_HANDLE;

		VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;


		bool CheckValidationLayerSupport(const std::vector<const char*> validationLayers);

		std::vector<const char*> GetRequiredExtensions(bool enableValidationLayers);

		bool CheckExtensionSupport(const char* extensionName);

		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData
		);

		void SetupDebugMessenger(bool enableValidationLayers);

		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	
		void DestroyDebugUtilsMessegerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
	};

}
#endif // !InstanceWrapperDefined