// CommanpoolManager.h
// This class will manage everything to do with commandpools

#ifndef CommandpoolManagerIncluded
#define CommandpoolManagerIncluded

// File includes
#include "Includes/VulkanIncludes.h"
#include "DataTypes/Structs.h"

// Standard library includes
#include <vector>

namespace D3D
{
	// Class forward declarations
	class GPUObject;

	class CommandpoolManager final
	{
	public:
		// Delete default constructor
		CommandpoolManager() = delete;

		// Constructor
		// Parameters:
		//     pGPUObject: pointer to the object that holds the physical and logical devices
		//     surface: handle of the VkSurfaceKHR
		CommandpoolManager(GPUObject* pGPUObject, VkSurfaceKHR surface);

		// Destructor
		~CommandpoolManager();

		// Delete the move and copy functions
		CommandpoolManager(CommandpoolManager& other) = delete;
		CommandpoolManager(CommandpoolManager&& other) = delete;
		CommandpoolManager& operator=(CommandpoolManager& other) = delete;
		CommandpoolManager& operator=(CommandpoolManager&& other) = delete;

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
		//     pGPUObject: pointer to the object that holds the physical and logical devices
		//     commandBuffer: handle of the commandbuffer in question
		void EndSingleTimeCommands(GPUObject* pGPUObject, VkCommandBuffer commandBuffer);

	private:
		//CommandPool
		VkCommandPool m_CommandPool{};

		//CommandBuffers
		std::vector<VkCommandBuffer> m_CommandBuffers{};

		// Initialize the commandpool
		// Parameters:
		//     pGPUObject: pointer to the object that holds the physical and logical devices
		//     surface: handle of th VkSurfaceKHR
		void CreateCommandPool(GPUObject* pGPUObject, VkSurfaceKHR surface);

		// Initialize the commandbuffers
		// Parameters:
		//     device: handle of the VkDevice
		void CreateCommandBuffers(VkDevice device);

		// Cleanup function
		// Parameters:
		//     device: handle of the VkDevice
		void Cleanup(VkDevice device);
	};
}



#endif // !CommandPoolManagerIncluded