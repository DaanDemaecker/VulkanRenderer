// GPUObject.cpp

// File includes
#include "GPUObject.h"

// Standard library includes

D2D::GPUObject::GPUObject(const VkInstance& instance)
{
	SetupPhysicalDevice(instance);
}

void D2D::GPUObject::SetupPhysicalDevice(const VkInstance& instance)
{
	std::vector<VkPhysicalDevice> physicalDevices{};

	EnumeratePhysicalDevices(instance, physicalDevices);
}

void D2D::GPUObject::EnumeratePhysicalDevices(const VkInstance& instance, std::vector<VkPhysicalDevice>& devices)
{
	VkResult result = VK_SUCCESS;

	uint32_t deviceCount{};

	result = vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (result == VK_SUCCESS)
	{
		devices.resize(deviceCount);

		result = vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	}
}
