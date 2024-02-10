#include "InstanceWrapper.h"

#include <stdexcept>
#include <iostream>

D3D::InstanceWrapper::InstanceWrapper(bool enableValidationLayers, const std::vector<const char *>& validationLayers, VkDebugUtilsMessengerEXT& debugMessenger)
{
#ifndef NDEBUG
	//If in debug and validation layers are requested, set up validation layers
	if (enableValidationLayers && !CheckValidationLayerSupport(validationLayers))
	{
		throw std::runtime_error("validation layers requested, but not available!");
	}
#endif

	//provide usefull information to the driver to optimize application
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan Renderer";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "D3D";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	//tell Vulkan driver which extensions and validation layers are wanted
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = GetRequiredExtensions(enableValidationLayers);
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		PopulateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}

	VkResult result = vkCreateInstance(&createInfo, nullptr, &m_Instance);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance");
	}

	SetupDebugMessenger(enableValidationLayers, debugMessenger);
}

void D3D::InstanceWrapper::cleanup(bool enableValidationLayers, VkDebugUtilsMessengerEXT& debugMessenger)
{
	if (enableValidationLayers)
	{
		DestroyDebugUtilsMessegerEXT(m_Instance, debugMessenger, nullptr);
	}

	vkDestroyInstance(m_Instance, nullptr);
}

bool D3D::InstanceWrapper::CheckValidationLayerSupport(const std::vector<const char *> validationLayers)
{
	uint32_t layerCount;

	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers)
	{
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
			return false;
	}

	return true;
}

std::vector<const char*> D3D::InstanceWrapper::GetRequiredExtensions(bool enableValidationLayers)
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	if (CheckExtensionSupport(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME))
	{
		extensions.push_back(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME);
	}

	return extensions;
}

bool D3D::InstanceWrapper::CheckExtensionSupport(const char* extensionName)
{
	uint32_t extensionCount;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

	for (const auto& extension : availableExtensions)
	{
		if (strcmp(extension.extensionName, extensionName) == 0)
		{
			return true;
		}
	}

	return false;
}

void D3D::InstanceWrapper::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
}

VKAPI_ATTR VkBool32 VKAPI_CALL D3D::InstanceWrapper::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT /*messageSeverity*/, VkDebugUtilsMessageTypeFlagsEXT /*messageType*/, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* /*pUserData*/)
{
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

void D3D::InstanceWrapper::SetupDebugMessenger(bool enableValidationLayers, VkDebugUtilsMessengerEXT& debugMessenger)
{
	if (!enableValidationLayers)
		return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	PopulateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

VkResult D3D::InstanceWrapper::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

	if (func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void D3D::InstanceWrapper::DestroyDebugUtilsMessegerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

	if (func != nullptr)
	{
		func(instance, debugMessenger, pAllocator);
	}
}