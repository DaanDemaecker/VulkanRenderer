// CommanpoolManager.h
// This class will manage everything to do with commandpools

#ifndef CommandpoolManagerIncluded
#define CommandpoolManagerIncluded

// File includes
#include "VulkanIncludes.h"
#include "Structs.h"

// Standard library includes
#include <vector>

namespace D3D
{
	class CommandpoolManager final
	{
	public:
		// Delete default constructor
		CommandpoolManager() = delete;

		// Constructor
		// Parameters:
		//     device: handle of the VkDevice
		//     physicalDevice: handle of the VkPhysicalDevice
		//     surface: handle of the VkSurfaceKHR
		//     frames: the max amount of frames in flight
		CommandpoolManager(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t frames);

		// Default destructor
		~CommandpoolManager() = default;

		CommandpoolManager(CommandpoolManager& other) = delete;
		CommandpoolManager(CommandpoolManager&& other) = delete;

		CommandpoolManager& operator=(CommandpoolManager& other) = delete;
		CommandpoolManager& operator=(CommandpoolManager&& other) = delete;

		// Cleanup function
		// Parameters:
		//     device: handle of the VkDevice
		void Cleanup(VkDevice device);

		// Get the current used command buffer
		// Parameters:
		//     frame: the current frame, to be used as the index for the command buffer
		VkCommandBuffer& GetCommandBuffer(uint32_t frame) { return m_CommandBuffers[frame]; }

		// Get a commandbuffer for a single use
		// Parameters:
		//     device: handle of the VkDevice
		VkCommandBuffer BeginSingleTimeCommands(VkDevice device);

		// End a commandbuffer that was for a single use
		// Parameters:
		//     device: handle of the VkDevice
		//     commandBuffer: handle of the commandbuffer in question
		//     graphicsQueue: handle of the graphics queue
		void EndSingleTimeCommands(VkDevice device, VkCommandBuffer commandBuffer, VkQueue graphicsQueue);

	private:
		//CommandPool
		VkCommandPool m_CommandPool{};

		//CommandBuffers
		std::vector<VkCommandBuffer> m_CommandBuffers{};

		// Initialize the commandpool
		// Parameters:
		//     device: handle of the VkDevice
		//     physicalDevice: handle of the VkPhysicalDevice
		//     surface: handle of th VkSurfaceKHR
		void CreateCommandPool(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

		// Find the needed queue families
		// Parameters:
		//     physicalDevice: handle of the VkPhysicalDevice
		//     surface: handle of the VkSurfaceKHR
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

		// Initialize the commandbuffers
		// Parameters:
		//     device: handle of the VkDevice
		//     frames: max amount of frames in flight
		void CreateCommandBuffers(VkDevice device, uint32_t frames);
	};
}



#endif // !CommandPoolManagerIncluded