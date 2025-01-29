// InstanceWrapper.cpp

// File includes
#include "InstanceWrapper.h"
#include "Engine/ConfigManager.h"

// Standard library includes
#include <stdexcept>

DDM2::InstanceWrapper::InstanceWrapper()
{
	CreateInstance();
}

void DDM2::InstanceWrapper::CreateInstance()
{
	VkInstanceCreateInfo createInfo{};

	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;

	VkApplicationInfo applicationInfo{};
	SetupApplicationInfo(applicationInfo);
	createInfo.pApplicationInfo = &applicationInfo;


	VkResult result = vkCreateInstance(&createInfo, nullptr, &m_Instance);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance");
	}
}

void DDM2::InstanceWrapper::SetupApplicationInfo(VkApplicationInfo& info)
{
	auto& configManager = ConfigManager::GetInstance();

	info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	info.pNext = nullptr;
	info.pApplicationName = configManager.GetString("ApplicationName");
	info.applicationVersion = VK_MAKE_API_VERSION(
		configManager.GetInt("ApplicationVersionVariant"),
		configManager.GetInt("ApplicationVersionMajor"),
		configManager.GetInt("ApplicationVersionMinor"),
		configManager.GetInt("ApplicationVersionPatch"));
	info.pEngineName = configManager.GetString("ApplicationName");
	info.engineVersion = VK_MAKE_API_VERSION(
		configManager.GetInt("EngineVersionVariant"),
		configManager.GetInt("EngineVersionMajor"),
		configManager.GetInt("EngineVersionMinor"),
		configManager.GetInt("EngineVersionPatch"));

	info.apiVersion = VK_API_VERSION_1_0;
}
