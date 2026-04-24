// VulkanCore.cpp

// Header include
#include "VulkanCore.h"

// File includes
#include "Engine/ConfigManager.h"

#include "Includes/VulkanIncludes.h"

#define GLFW_INCLUDE_VULKAN
#include "Includes/GlfwIncludes.h"

#include "Engine/Window/Window.h"
#include "Vulkan/Core/VulkanAllocator.h"

// Standard library includes
#include <set>

bool DDM::VulkanCore::m_LogError{ false };
bool DDM::VulkanCore::m_LogWarning{ false };
bool DDM::VulkanCore::m_LogInfo{ false };
bool DDM::VulkanCore::m_LogVerbose{ false };

DDM::VulkanCore::VulkanCore(VulkanAllocator* pAllocator)
{
	m_pAllocator = pAllocator;

	// Window must be initialized BEFORE creating instance
	Window::GetInstance();

	CreateInstance();

	SetupDebugMessenger();

	CreateSurface();

	SetupPhysicalDevice();

	CreateLogicalDevice();
}

DDM::VulkanCore::~VulkanCore()
{
	vkDeviceWaitIdle(m_VkDevice);

	vkDestroyDevice(m_VkDevice, m_pAllocator->GetAllocator());

	vkDestroySurfaceKHR(m_VkInstance, m_VkSurface, m_pAllocator->GetAllocator());

	if (m_VkDebugMessenger != VK_NULL_HANDLE)
	{
		DestroyDebugMessenger(m_VkInstance, m_VkDebugMessenger, m_pAllocator->GetAllocator());
	}

	vkDestroyInstance(m_VkInstance, m_pAllocator->GetAllocator());
}

// ------------------------------------------------------------------------------
// Instance
//-------------------------------------------------------------------------------

void DDM::VulkanCore::CreateInstance()
{
	VkInstanceCreateInfo createInfo{};

	SetupValidationLayers(createInfo);

	auto applicationInfo = GetApplicationInfo();

	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.flags = 0;

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	createInfo.pNext = nullptr;

	if (m_EnableValidationLayers)
	{
		PopulateDebugMessenger(debugCreateInfo);
		createInfo.pNext = &debugCreateInfo;
	}


	createInfo.pApplicationInfo = &applicationInfo;

	
	auto extensions = GetExtensions();

	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	if (vkCreateInstance(&createInfo, m_pAllocator->GetAllocator(), &m_VkInstance) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to set vulkan instance!");
	}
}

VkApplicationInfo DDM::VulkanCore::GetApplicationInfo()
{
	auto& configManager = DDM::ConfigManager::GetInstance();

	VkApplicationInfo info{};

	info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	info.pNext = nullptr;
	info.pApplicationName = configManager.GetString("ApplicationName");
	info.applicationVersion = VK_MAKE_API_VERSION(
		configManager.GetInt("ApplicationVersionVariant"),
		configManager.GetInt("ApplicationVersionMajor"),
		configManager.GetInt("ApplicationVersionMinor"),
		configManager.GetInt("ApplicationVersionPatch")
	);

	info.pEngineName = configManager.GetString("EngineName");
	info.engineVersion = VK_MAKE_API_VERSION(
		configManager.GetInt("EngineVersionVariant"),
		configManager.GetInt("EngineVersionMajor"),
		configManager.GetInt("EngineVersionMinor"),
		configManager.GetInt("EngineVersionPatch")
	);

	info.apiVersion = VK_API_VERSION_1_3;

	return info;
}

void DDM::VulkanCore::SetupValidationLayers(VkInstanceCreateInfo& createInfo)
{
#ifdef NDEBUG
	m_EnableValidationLayers = false;
#else
	m_EnableValidationLayers = true;
#endif

	if (!m_EnableValidationLayers)
	{
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;

		return;
	}

	if (!QueryValidationLayerSupport())
	{
		throw std::runtime_error("Validation layers requested, but not available!");
	}

	createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
	createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
}

bool DDM::VulkanCore::QueryValidationLayerSupport()
{
	uint32_t layerCount;

	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : m_ValidationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

std::vector<const char*> DDM::VulkanCore::GetExtensions()
{
	uint32_t glfwExtensionCount = 0;

	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (m_EnableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

void DDM::VulkanCore::SetupDebugMessenger()
{
	if (!m_EnableValidationLayers)
	{
		return;
	}

	auto& config = ConfigManager::GetInstance();

	m_LogError = config.GetBool("LogValidationError");


	m_LogWarning = config.GetBool("LogValidationWarning");


	m_LogInfo = config.GetBool("LogValidationInfo");


	m_LogVerbose = config.GetBool("LogValidationVerbose");

	VkDebugUtilsMessengerCreateInfoEXT createInfo{};

	PopulateDebugMessenger(createInfo);

	if (CreateDebugMessenger(m_VkInstance, &createInfo, m_pAllocator->GetAllocator(), &m_VkDebugMessenger) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

void DDM::VulkanCore::PopulateDebugMessenger(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;

	SetupDebugMessengerSeverities(createInfo);

	SetupDebugMessengerTypes(createInfo);

	createInfo.pfnUserCallback = debugCallback;

	createInfo.pUserData = nullptr;
}

void DDM::VulkanCore::SetupDebugMessengerSeverities(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo.messageSeverity = 0;
	
	auto& config = ConfigManager::GetInstance();

	if (config.GetBool("ValidationSeverityVerbose"))
	{
		createInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
	}

	if (config.GetBool("ValidationSeverityInfo"))
	{
		createInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
	}

	if (config.GetBool("ValidationSeverityWarning"))
	{
		createInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
	}

	if (config.GetBool("ValidationSeverityError"))
	{
		createInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	}
}

void DDM::VulkanCore::SetupDebugMessengerTypes(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
}

VkResult DDM::VulkanCore::CreateDebugMessenger(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
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

void DDM::VulkanCore::DestroyDebugMessenger(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

	if (func != nullptr)
	{
		func(instance, debugMessenger, pAllocator);
	}
}


VKAPI_ATTR VkBool32 VKAPI_CALL DDM::VulkanCore::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	switch (messageSeverity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		if (m_LogVerbose)
		{
			std::cout << "Validation layer verbose: " << pCallbackData->pMessage << "\n \n";
		}
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		if (m_LogInfo)
		{
			std::cout << "Validation layer info: " << pCallbackData->pMessage << "\n \n";
		}
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		if (m_LogWarning)
		{
			std::cout << "Validation layer warning: " << pCallbackData->pMessage << "\n \n";
		}
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		if (m_LogError)
		{
			std::cout << "Validation layer error: " << pCallbackData->pMessage << "\n \n";
		}
		break;
	default:
		break;
	}

	return VK_FALSE;
}


// ------------------------------------------------------------------------------
// Surface
// ------------------------------------------------------------------------------

void DDM::VulkanCore::CreateSurface()
{
	if (!glfwVulkanSupported())
	{
		throw std::runtime_error("Vulkan is not supported by glfw");
	}

	VkResult result = glfwCreateWindowSurface(m_VkInstance,
		static_cast<GLFWwindow*>(DDM::Window::GetInstance().GetWindowData().handle),
		m_pAllocator->GetAllocator(), &m_VkSurface);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create window surface");
	}
}


// ------------------------------------------------------------------------------
// Physical device
//-------------------------------------------------------------------------------

void DDM::VulkanCore::SetupPhysicalDevice()
{
	m_VkPhysicalDevice = PickPhysicalDevice();

	if (m_VkPhysicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("Failed to pick a physical device");
	}
}

VkPhysicalDevice DDM::VulkanCore::PickPhysicalDevice()
{
	// Enumerate all devices
	uint32_t deviceCount{};

	VkResult result = vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, nullptr);

	if (result != VK_SUCCESS || deviceCount == 0)
	{
		throw std::runtime_error("failed to enumerate physical devices!");
	}

	std::vector<VkPhysicalDevice> physicalDevices = std::vector<VkPhysicalDevice>(deviceCount);
	
	result = vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, physicalDevices.data());

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to enumerate physical devices!");
	}

	// Score all physical devices and pick highest scored one
	uint32_t maxScore = 0;

	VkPhysicalDevice chosenDevice = VK_NULL_HANDLE;

	for (auto physicalDevice : physicalDevices)
	{
		if (!IsDeviceValid(physicalDevice))
		{
			continue;
		}

		uint32_t score = ScorePhysicalDevice(physicalDevice);

		if (score > maxScore)
		{
			maxScore = score;

			chosenDevice = physicalDevice;
		}
	}

	return chosenDevice;
}

uint32_t DDM::VulkanCore::ScorePhysicalDevice(VkPhysicalDevice physicalDevice)
{
	constexpr uint32_t baseScore = 1;

	uint32_t score = baseScore;

	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(physicalDevice, &properties);

	// Device type
	// Take a preference of discrete gpu over integrated gpu and prefer both of these over any other type
	constexpr uint32_t integratedMultiplier = 2;
	constexpr uint32_t discreteMultiplier = 3;

	if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
	{
		score *= integratedMultiplier;
	}
	else if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
	{
		score *= discreteMultiplier;
	}

	std::cout << "Score of gpu " << properties.deviceName << ": " << score << std::endl;

	return score;
}

bool DDM::VulkanCore::IsDeviceValid(VkPhysicalDevice physicalDevice)
{
	if (!HasRequiredExtensions(physicalDevice))
	{
		return false;
	}

	if (!HasRequiredQueueFamily(physicalDevice))
	{
		return false;
	}

	return true;
}

bool DDM::VulkanCore::HasRequiredExtensions(VkPhysicalDevice physicalDevice)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
	
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

	//Create a set of required extensions to avoid duplicates
	std::set<std::string> requiredExtensions(m_RequiredExtensions.begin(), m_RequiredExtensions.end());

	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

bool DDM::VulkanCore::HasRequiredQueueFamily(VkPhysicalDevice physicalDevice)
{
	std::vector<VkQueueFamilyProperties> queueFamilies{};

	GetQueueFamilies(physicalDevice, queueFamilies);

	for (auto& family : queueFamilies)
	{
		if (IsValidQueueFamily(family))
		{
			return true;
		}
	}

	return false;
}

bool DDM::VulkanCore::IsValidQueueFamily(VkQueueFamilyProperties family)
{
	for (auto bit : m_RequiredQueueFlags)
	{
		if ((family.queueFlags & bit) == 0)
		{
			return false;
		}
	}

	return true;
}

void DDM::VulkanCore::GetQueueFamilies(VkPhysicalDevice device, std::vector<VkQueueFamilyProperties>& families)
{
	uint32_t queueFamilyCount{};

	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	families.resize(queueFamilyCount);

	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, families.data());
}


// ------------------------------------------------------------------------------
// Logical device
//-------------------------------------------------------------------------------

void DDM::VulkanCore::CreateLogicalDevice()
{
	VkDeviceCreateInfo deviceCreateInfo{};

	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = nullptr;
	deviceCreateInfo.flags = 0;


	// Set up queue create info
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfo{};
	std::vector<std::vector<float>> queuePriorities{};

	SetupQueueCreateInfos(queueCreateInfo, queuePriorities);

	deviceCreateInfo.pQueueCreateInfos = queueCreateInfo.data();
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfo.size());


	// Device layers are depricated in modern vulkan
	deviceCreateInfo.enabledLayerCount = 0;
	deviceCreateInfo.ppEnabledLayerNames = nullptr;

	// Set up extensions info
	deviceCreateInfo.ppEnabledExtensionNames = m_RequiredExtensions.data();
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_RequiredExtensions.size());

	// Set up features info
	VkPhysicalDeviceFeatures features{};

	SetupPhysicalDeviceFeatures(features);

	deviceCreateInfo.pEnabledFeatures = &features;

	// Create the device
	vkCreateDevice(m_VkPhysicalDevice, &deviceCreateInfo, m_pAllocator->GetAllocator(), &m_VkDevice);
}

void DDM::VulkanCore::SetupQueueCreateInfos(std::vector<VkDeviceQueueCreateInfo>& infos, std::vector<std::vector<float>>& priorities)
{
	VkDeviceQueueCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;

	FindOptimalQueueFamily(info.queueFamilyIndex, info.queueCount);

	priorities.push_back(std::vector<float>());

	for (uint32_t i{}; i < info.queueCount; ++i)
	{
		priorities[0].push_back(1.0f);
	}

	info.pQueuePriorities = priorities[0].data();

	infos.push_back(info);
}

void DDM::VulkanCore::FindOptimalQueueFamily(uint32_t& index, uint32_t& count)
{
	index = 0;
	count = 0;

	std::vector<VkQueueFamilyProperties> queueFamilies{};

	GetQueueFamilies(m_VkPhysicalDevice, queueFamilies);

	for (int i{}; i < queueFamilies.size(); ++i)
	{
		if (IsValidQueueFamily(queueFamilies[i]))
		{
			if (queueFamilies[i].queueCount > count)
			{
				count = queueFamilies[i].queueCount;
				index = i;
			}
		}
	}
}

void DDM::VulkanCore::SetupPhysicalDeviceFeatures(VkPhysicalDeviceFeatures& features)
{
	// Enable all available features
	vkGetPhysicalDeviceFeatures(m_VkPhysicalDevice, &features);
}
