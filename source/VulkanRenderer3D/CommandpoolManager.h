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
	class CommandpoolManager
	{
	public:
		CommandpoolManager() = delete;

		CommandpoolManager(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t frames);

		~CommandpoolManager() = default;

		CommandpoolManager(CommandpoolManager& other) = delete;
		CommandpoolManager(CommandpoolManager&& other) = delete;

		CommandpoolManager& operator=(CommandpoolManager& other) = delete;
		CommandpoolManager& operator=(CommandpoolManager&& other) = delete;

		void Cleanup(VkDevice device);

		VkCommandBuffer& GetCommandBuffer(uint32_t frame) { return m_CommandBuffers[frame]; }

		void EndSingleTimeCommands(VkDevice device, VkCommandBuffer comandBuffer, VkQueue graphicsQueue);

		VkCommandBuffer BeginSingleTimeCommands(VkDevice device);


	private:
		//--CommandPool--
		VkCommandPool m_CommandPool{};

		//CommandBuffers
		std::vector<VkCommandBuffer> m_CommandBuffers{};

		void CreateCommandPool(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
		void CreateCommandBuffers(VkDevice device, uint32_t frames);
	};
}



#endif // !CommandPoolManagerIncluded