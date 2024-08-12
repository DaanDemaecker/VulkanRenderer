// SyncObjectManager.h
// This class wil hold and manage the vulkan sync objects

#ifndef SyncObjectManagerIncluded
#define SyncObjectManagerIncluded

// File includes
#include "Includes/VulkanIncludes.h"

// Standard library includes
#include <vector>

namespace D3D
{
	class SyncObjectManager final
	{
	public:
		// Delete default constructor
		SyncObjectManager() = delete;

		// Constructor
		// Parameters:
		//     device: handle of the VkDevice
		SyncObjectManager(VkDevice device);

		// Default destructor
		~SyncObjectManager();

		// Delete copy and move functions
		SyncObjectManager(SyncObjectManager& other) = delete;
		SyncObjectManager(SyncObjectManager&& other) = delete;
		SyncObjectManager& operator=(SyncObjectManager& other) = delete;
		SyncObjectManager& operator=(SyncObjectManager&& other) = delete;

		// Get the requested image available semaphore
		// Parameters:
		//     frame: current frame and index of the semaphore
		VkSemaphore& GetImageAvailableSemaphore(uint32_t frame) { return m_ImageAvailableSemaphores[frame]; }

		// Get the requested render finished semaphore
		// Parameters:
		//     frame: current frame and index of the semaphore
		VkSemaphore& GetRenderFinishedSemaphore(uint32_t frame) { return m_RenderFinishedSemaphores[frame]; }

		// Get the requested in flight fence
		// Parameters:
		//     frame: current frame and index of the fence
		VkFence& GetInFlightFence(uint32_t frame) { return m_InFlightFences[frame]; }

	private:

		// Vector of image available semaphores
		std::vector<VkSemaphore> m_ImageAvailableSemaphores{};

		// Vector of render finished semaphores
		std::vector<VkSemaphore> m_RenderFinishedSemaphores{};

		// Vector of in flight fences
		std::vector<VkFence> m_InFlightFences{};

		// Initialize the sync objects
		// Paramters:
		//     device: handle of the VkDevice
		void CreateSyncObjects(VkDevice device);

		// Clean up allocated objects
		void Cleanup(VkDevice device);
	};
}
#endif // !SyncObjectManagerIncluded