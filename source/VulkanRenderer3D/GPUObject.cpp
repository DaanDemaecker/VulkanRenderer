// GPUObject.h

// File includes
#include "GPUObject.h"
#include "VulkanUtils.h"
#include "InstanceWrapper.h"

// Standard library includes
#include <stdexcept>
#include <map>
#include <set>

D3D::GPUObject::GPUObject(InstanceWrapper* pInstanceWrapper, VkSurfaceKHR surface)
{
	// Pick the physical device
	PickPhysicalDevice(pInstanceWrapper, surface);

	// Create the logical device
	CreateLogicalDevice(pInstanceWrapper, surface);
}



void D3D::GPUObject::CleanUp()
{
	// Destroy the logical device
	vkDestroyDevice(m_Device, nullptr);
}

void D3D::GPUObject::WaitIdle()
{
	// Wait until the device is idle
	vkDeviceWaitIdle(m_Device);
}

void D3D::GPUObject::PickPhysicalDevice(InstanceWrapper* pInstanceWrapper, VkSurfaceKHR surface)
{
	//Get number of physical devices that support Vulkan
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(pInstanceWrapper->GetInstance(), &deviceCount, nullptr);

	//If no physical devices found, throw runtime error
	if (deviceCount == 0)
	{
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	//Allocate array to hold physical devices handles
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(pInstanceWrapper->GetInstance(), &deviceCount, devices.data());

	//use an ordered map to automatically sort candidates by increasing score
	std::multimap<int, VkPhysicalDevice> candidates;

	// Loop trough all available physical devices
	for (const auto& device : devices)
	{
		// Check if the device is suitable
		if (IsDeviceSuitable(device, surface))
		{
			// Set the physical device to the current one and break the loop
			m_PhysicalDevice = device;
			break;
		}
	}


	// Check if physical device is null handle
	if (m_PhysicalDevice == VK_NULL_HANDLE)
	{
		// If null handle, throw runtime error
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

bool D3D::GPUObject::IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	// Get the queue families for the given physical device and surface
	QueueFamilyIndices indices = VulkanUtils::FindQueueFamilies(device, surface);

	// Check if the device supports all requested extensions
	bool extensionsSupported = CheckDeviceExtensionSupport(device);

	// Boolean for adequaty of the swapchain
	bool swapChainAdequate = false;

	// If the extensions are supported
	if (extensionsSupported)
	{
		// Get the swapchain support details
		SwapChainSupportDetails swapChainSupport = VulkanUtils::QuerySwapChainSupport(device, surface);
		// If formats and presentmodes are not empty, set swapChainAdequate to true
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	// Get the physical device features
	VkPhysicalDeviceFeatures supportedFeatures{};
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	// Return true if:
	// The device queue families are complete
	// The extensions are all supported
	// The swapchain is adequate
	// Sampler anisotropy is supported
	// Geometry shaders are supported
	return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy
		&& supportedFeatures.geometryShader;
}


bool D3D::GPUObject::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
	//Check how many extensions this device supports
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	//Get a list of all available extensions
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	//Create a set of required extensions to avoid duplicates
	std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

	//Clear all available extensions from the required ones
	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	//If the required extensions are empty, they are all available
	return requiredExtensions.empty();
}


void D3D::GPUObject::CreateLogicalDevice(InstanceWrapper* pInstanceWrapper, VkSurfaceKHR surface)
{
	// Get the suited queue family indices
	QueueFamilyIndices indices = VulkanUtils::FindQueueFamilies(m_PhysicalDevice, surface);

	// Create vector for queue create infos
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	// Create vector for the unique families and initialize with the graphics- and present family queues
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	// Set the graphics family queue to the value of the graphics family index
	m_QueueObject.graphicsQueueIndex = indices.graphicsFamily.value();

	// Initialize queuePriority with 1
	float queuePriority = 1.0f;
	// Loop trough all the unique families
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		// Create queue create info
		VkDeviceQueueCreateInfo queueCreateInfo{};
		// Set type to queue create info
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		// Set queue family index to the current queue family
		queueCreateInfo.queueFamilyIndex = queueFamily;
		// Set queue count to 1
		queueCreateInfo.queueCount = 1;
		// Give the queue priority
		queueCreateInfo.pQueuePriorities = &queuePriority;
		// Place the create info in the vector of create infos
		queueCreateInfos.push_back(queueCreateInfo);
	}

	// Initialize device features object
	VkPhysicalDeviceFeatures deviceFeatures{};
	// Enable anisotropy sampler
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	// Enable sampler rate shading
	deviceFeatures.sampleRateShading = VK_TRUE;

	// Create device create info
	VkDeviceCreateInfo createInfo{};
	// Set type to device create info
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	// Set the amount of queue create infos to the size of the vector of create infos
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	// Give pointer to data of queue create info vector
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	// Give the requested device features
	createInfo.pEnabledFeatures = &deviceFeatures;
	// Set amount of extensions to the size of the extensions vector
	createInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
	// Give pointer to data of extensions vector
	createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

	// Check if validation layers are enabled
	if (pInstanceWrapper->ValidationLayersEnabled())
	{   // If enabled
		// Set the amount of validation layers as the size of the validation layer vector
		createInfo.enabledLayerCount = static_cast<uint32_t>(pInstanceWrapper->GetValidationLayers().size());
		// Give pointer to data of the validation layer vector
		createInfo.ppEnabledLayerNames = pInstanceWrapper->GetValidationLayers().data();
	}
	else
	{
		// If not enabled, set layer count to 0
		createInfo.enabledLayerCount = 0;
	}

	// Create the logical device
	if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to create logical device!");
	}

	// Get the graphics queue
	vkGetDeviceQueue(m_Device, indices.graphicsFamily.value(), 0, &m_QueueObject.graphicsQueue);
	// Get the present queue
	vkGetDeviceQueue(m_Device, indices.presentFamily.value(), 0, &m_QueueObject.presentQueue);
}