// SyncObjectManager.cpp

// File includes
#include "SyncObjectManager.h"

// Standard library includes
#include <stdexcept>

D3D::SyncObjectManager::SyncObjectManager(VkDevice device, uint32_t maxFrames)
{
	// Initialize the sync objects
	CreateSyncObjects(device, maxFrames);
}

void D3D::SyncObjectManager::Cleanup(VkDevice device)
{
	// Loop trough the amount of fences and semaphores
	for (size_t i = 0; i < m_ImageAvailableSemaphores.size(); ++i)
	{
		// Destroy image available semaphore
		vkDestroySemaphore(device, m_ImageAvailableSemaphores[i], nullptr);
		// Destroy render finished semaphore
		vkDestroySemaphore(device, m_RenderFinishedSemaphores[i], nullptr);
		// Destroy in flight fence
		vkDestroyFence(device, m_InFlightFences[i], nullptr);
	}
}

void D3D::SyncObjectManager::CreateSyncObjects(VkDevice device, uint32_t maxFrames)
{
	// Resize image available semaphore to the amound of frames
	m_ImageAvailableSemaphores.resize(maxFrames);
	// Resize render finished semaphore to the amount of frames
	m_RenderFinishedSemaphores.resize(maxFrames);
	// Resize the in fligh fences to the amount of frames
	m_InFlightFences.resize(maxFrames);

	// Create semaphore create info
	VkSemaphoreCreateInfo semaphoreInfo{};
	// Set type to semaphore create info
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	// Create fence create info
	VkFenceCreateInfo fenceInfo{};
	// Set type to fence create info
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	// Set flags to signaled
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	// Loop trough the amound of frames
	for (size_t i = 0; i < maxFrames; ++i)
	{
		// Create the sync objects
		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device, &fenceInfo, nullptr, &m_InFlightFences[i]) != VK_SUCCESS)
		{
			// If unsuccesful, throw runtime error
			throw std::runtime_error("failed to create sync objects!");
		}
	}
}