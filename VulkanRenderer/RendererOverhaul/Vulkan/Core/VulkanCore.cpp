// VulkanCore.cpp

// Header include
#include "VulkanCore.h"
#include "Engine/ConfigManager.h"

#include "Includes/VulkanIncludes.h"
#include "Includes/GLFWIncludes.h"

DDM::VulkanCore::VulkanCore()
{
	CreateInstance();

	SetupDebugMessenger();

	SetupPhysicalDevice();
}

DDM::VulkanCore::~VulkanCore()
{
	if (m_VkDebugMessenger != VK_NULL_HANDLE)
	{
		DestroyDebugMessenger(m_VkInstance, m_VkDebugMessenger, nullptr);
	}


	vkDestroyInstance(m_VkInstance, nullptr);
}

void DDM::VulkanCore::CreateInstance()
{
	VkInstanceCreateInfo createInfo{};

	SetupValidationLayers(createInfo);

	auto applicationInfo = GetApplicationInfo();

	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.flags = 0;

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

	if (m_EnableValidationLayers)
	{
		PopulateDebugMessenger(debugCreateInfo);
		createInfo.pNext = &debugCreateInfo;
	}


	createInfo.pNext = nullptr;
	createInfo.pApplicationInfo = &applicationInfo;

	
	auto extensions = GetExtensions();

	createInfo.enabledExtensionCount = extensions.size();
	createInfo.ppEnabledExtensionNames = extensions.data();

	if (vkCreateInstance(&createInfo, nullptr, &m_VkInstance) != VK_SUCCESS)
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

		for (const auto& layerProperties : availableLayers) {
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

	auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

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

	VkDebugUtilsMessengerCreateInfoEXT createInfo{};

	PopulateDebugMessenger(createInfo);

	if (CreateDebugMessenger(m_VkInstance, &createInfo, nullptr, &m_VkDebugMessenger) != VK_SUCCESS)
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
	std::cout << "Validation layer error: " << pCallbackData->pMessage << std::endl;


	return VK_FALSE;
}

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
