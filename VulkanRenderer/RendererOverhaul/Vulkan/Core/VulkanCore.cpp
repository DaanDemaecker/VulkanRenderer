// VulkanCore.cpp

// Header include
#include "VulkanCore.h"
#include "Engine/ConfigManager.h"

DDM::VulkanCore::VulkanCore()
{
	CreateInstance();
}

DDM::VulkanCore::~VulkanCore()
{
	vkDestroyInstance(m_VkInstance, nullptr);
}

void DDM::VulkanCore::CreateInstance()
{
	VkInstanceCreateInfo createInfo{};

	auto applicationInfo = GetApplicationInfo();

	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.flags = 0;
	createInfo.pNext = nullptr;
	createInfo.pApplicationInfo = &applicationInfo;

	SetupValidationLayers(createInfo);

	createInfo.enabledExtensionCount = 0;
	createInfo.ppEnabledExtensionNames = nullptr;

	vkCreateInstance(&createInfo, nullptr, &m_VkInstance);
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
