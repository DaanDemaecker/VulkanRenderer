// DescriptorPoolManager.h
// This class holds all the descriptorpools and can provide the correct one when requested

#ifndef DescriptorPoolManagerIncluded
#define DescriptorPoolManagerIncluded

// File includes
#include "VulkanIncludes.h"

// Standard library includes
#include <utility>
#include <map>
#include <memory>

namespace D3D
{
	// Class forward declaration for descriptorpool wrapper
	class DescriptorPoolWrapper;

	class DescriptorPoolManager
	{
	public:
		// Default constructor
		DescriptorPoolManager() = default;
		// Default destructor
		~DescriptorPoolManager() = default;

		DescriptorPoolManager(DescriptorPoolManager& other) = delete;
		DescriptorPoolManager(DescriptorPoolManager&& other) = delete;

		DescriptorPoolManager& operator=(DescriptorPoolManager& other) = delete;
		DescriptorPoolManager& operator=(DescriptorPoolManager&& other) = delete;


		// This function will clean up all descriptorpool
		// This isn't done in the destructor as the order of objects being deleted is very important in Vulkan
		// Parameters:
		//     device: the vulkan logical device
		void Cleanup(VkDevice device);

		// This function will return the requested descriptorpool
		// Parameters:
		//     uboAmount: the amount of universal buffer objects per object in this descriptorpool
		//     textureAmount: the amount of textures per object in this descriptorpool
		D3D::DescriptorPoolWrapper* GetDescriptorPool(uint32_t uboAmount, uint32_t textureAmount);

	private:
		// Alocated pair to avoid creating new pair every request
		std::pair<uint32_t, uint32_t> m_Pair{ std::make_pair<uint32_t, uint32_t>(0, 0) };

		// Map of all descriptorpool wrappers
		std::map<std::pair<uint32_t, uint32_t>, std::unique_ptr<DescriptorPoolWrapper>> m_pDescriptorPools;
	};
}

#endif // !DescriptorPoolManagerIncluded