// GPUObject.h
// This object will hold the main objects that talk to the GPU like the VkDevice and VkPhysicalDevice

#ifndef GPUObjectIncluded
#define GPUObjectIncluded

// File includes
#include "VulkanIncludes.h"
#include "Structs.h"

// Standard library includes
#include <vector>

namespace D3D
{
	class InstanceWrapper;

	class GPUObject
	{
	public:
		GPUObject(InstanceWrapper* pInstanceWrapper, VkSurfaceKHR surface);

		GPUObject() = delete;

		~GPUObject() = default;

		GPUObject(GPUObject& other) = delete;
		GPUObject(GPUObject&& other) = delete;

		GPUObject& operator=(GPUObject& other) = delete;
		GPUObject& operator=(GPUObject&& other) = delete;

		void CleanUp();
		
		void WaitIdle();

		VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
		VkDevice GetDevice() const { return m_Device; }
		const QueueObject& GetQueueObject() const { return m_QueueObject; }


	private:
		// Handle of the VkPhysicalDevice
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;

		// Vector of requested device extensions
		const std::vector<const char*> m_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		// Handle of the logical device
		VkDevice m_Device = VK_NULL_HANDLE;

		// Object that holds the graphics and present family queues
		QueueObject m_QueueObject{};


		// Pick the physical device
		void PickPhysicalDevice(InstanceWrapper* pInstanceWrapper, VkSurfaceKHR surface);

		// Check if a given device is suitable
		// Parameters:
		//     device: the device to be checked
		bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR);

		// Check if a given device satisfies the extensions
		// Parameters:
		//     device: the device to be checked
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

		// Initialize the logical device
		void CreateLogicalDevice(InstanceWrapper* pInstanceWrapper, VkSurfaceKHR surface);
	};
}


#endif // !GPUObjectIncluded