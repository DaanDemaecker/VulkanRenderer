// InstaceWrapper.cpp

// File includes
#include "InstanceWrapper.h"  
#include "Includes/GLFWIncludes.h"
#include "Vulkan/VulkanUtils.h"
#include "Engine/ConfigManager.h"

// Standard library includes
#include <stdexcept>
#include <iostream>

D3D::InstanceWrapper::InstanceWrapper()
{
#ifdef NDEBUG
	// If in release mode, disable validation layers
	m_EnableValidationLayers = false;
#endif

#ifndef NDEBUG
	//If in debug and validation layers are requested, set up validation layers
	if (m_EnableValidationLayers && !CheckValidationLayerSupport(m_ValidationLayers))
	{
		// If validation layers are not supported, throw runtime error
		throw std::runtime_error("validation layers requested, but not available!");
	}
#endif

	//provide usefull information to the driver to optimize application
	VkApplicationInfo appInfo{};
	std::string applicationName{}, engineName{};
	// Setup application info
	SetupApplicationInfo(appInfo, applicationName, engineName);

	// Create instance create info
	VkInstanceCreateInfo createInfo{};
	// Set type to instance create info
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	// Give pointer to app info
	createInfo.pApplicationInfo = &appInfo;

	// Get required extensions
	auto extensions = GetRequiredExtensions(m_EnableValidationLayers);
	// Set extension count to size of extensions
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	// Give data of extensions
	createInfo.ppEnabledExtensionNames = extensions.data();

	// Create debug create info
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (m_EnableValidationLayers)
	{
		// If validation layers enabled:
		// Set layercount to validation layers size
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
		// Set enabled layers names to data of validation layers
		createInfo.ppEnabledLayerNames = m_ValidationLayers.data();

		// Pupulate the debug messenger create info
		PopulateDebugMessengerCreateInfo(debugCreateInfo);
		// Set pNext to a pointer to debug create info
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else
	{
		// If validation layers are disabled:
		// Set layercount to 0
		createInfo.enabledLayerCount = 0;
		// Set pNext to nullptr
		createInfo.pNext = nullptr;
	}

	// Create vulkan instance
	if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to create instance");
	}

	// Set up debug messenger
	SetupDebugMessenger(m_EnableValidationLayers);
}

D3D::InstanceWrapper::~InstanceWrapper()
{
	// If validation layers enabled, delete debug messenger
	if (m_EnableValidationLayers)
	{
		DestroyDebugUtilsMessegerEXT(m_Instance, m_DebugMessenger, nullptr);
	}

	// Destroy instance
	vkDestroyInstance(m_Instance, nullptr);
}

void D3D::InstanceWrapper::SetupApplicationInfo(VkApplicationInfo& appInfo, std::string& applicationName, std::string& engineName)
{
	// Get config manager
	auto& configManager{ ConfigManager::GetInstance() };

	// Set type to application info
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	// Get name of application from config
	applicationName = configManager.GetString("ApplicationName");
	appInfo.pApplicationName = applicationName.c_str();
	// Set version of application
	appInfo.applicationVersion = VK_MAKE_VERSION(configManager.GetInt("ApplicationVersionMajor"), 
		configManager.GetInt("ApplicationVersionMinor"), 
		configManager.GetInt("ApplicationVersionPatch"));
	// Get name of engine from config
	engineName = configManager.GetString("EngineName");
	appInfo.pEngineName = engineName.c_str();
	// Set version of engine
	appInfo.engineVersion = VK_MAKE_VERSION(configManager.GetInt("EngineVersionMajor"),
		configManager.GetInt("EngineVersionMinor"),
		configManager.GetInt("EngineVersionPatch"));
	// Set version of api
	appInfo.apiVersion = VK_API_VERSION_1_0;
}

bool D3D::InstanceWrapper::CheckValidationLayerSupport(const std::vector<const char *> validationLayers)
{
	// Create layercount
	uint32_t layerCount{};

	// Get layercount
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	// Create vector for layers the size of layercount
	std::vector<VkLayerProperties> availableLayers(layerCount);
	// Get the available layers
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	// Loop trough all the validation layers
	for (const char* layerName : validationLayers)
	{
		// Create layerfound boolean
		bool layerFound = false;

		// Loop trough all the available layers
		for (const auto& layerProperties : availableLayers)
		{
			// Compare the strings
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				// If the strings are equal, the layer has been found
				layerFound = true;
				// Stop the loop trough available layers
				break;
			}
		}

		// If the layer wasn't found, return false
		if (!layerFound)
			return false;
	}

	// If all layers were found, return true
	return true;
}

std::vector<const char*> D3D::InstanceWrapper::GetRequiredExtensions(bool enableValidationLayers)
{
	// Crate uint fro the amount of glfw extensions
	uint32_t glfwExtensionCount = 0;
	// Create char** for the names of the glfw estensions
	const char** glfwExtensions;

	// Get the glfw extensions and their count
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	// Create a vector to hold the extensions, starting from the address of glfwExtensions and ending at glfwExtensions + the amount of extensions
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	// If the validation layers are enabled
	if (enableValidationLayers)
	{
		// Add the debut utils extension
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	// If the push descriptor is supported, add it to the extensions list
	if (VulkanUtils::CheckExtensionSupport(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME))
	{
		extensions.push_back(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME);
	}

	// Return the extensions
	return extensions;
}

void D3D::InstanceWrapper::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	// Set type to messenger create info
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	// Only allow message severity of warning or error level
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	// Only allow message types of general, validation and performance
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	// Give the debug callback function adress
	createInfo.pfnUserCallback = debugCallback;
}

VKAPI_ATTR VkBool32 VKAPI_CALL D3D::InstanceWrapper::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT /*messageSeverity*/, VkDebugUtilsMessageTypeFlagsEXT /*messageType*/, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* /*pUserData*/)
{
	// Print the validation layer error
	std::cerr << "validation layer error: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

void D3D::InstanceWrapper::SetupDebugMessenger(bool enableValidationLayers)
{
	// If validation layers aren't enabled, return
	if (!enableValidationLayers)
		return;

	// Create messenger create info
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	// Setup messenger crerate info
	PopulateDebugMessengerCreateInfo(createInfo);

	// Create the debugMessenger
	if (CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

VkResult D3D::InstanceWrapper::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	// Get the adress of the debug messenger creation function
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

	// If the function is still a nullptr, the extension is not present
	if (func != nullptr)
	{
		// If the function is available, run it and return the result
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else
	{
		// If the function is not present, return error
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void D3D::InstanceWrapper::DestroyDebugUtilsMessegerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
	// Get the adress of the debug messenger delete function
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

	// If the function is still a nullptr, the extension is not present
	if (func != nullptr)
	{
		// If the function is available, run it
		func(instance, debugMessenger, pAllocator);
	}
}