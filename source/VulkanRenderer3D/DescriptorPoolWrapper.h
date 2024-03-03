// DescriptorPoolWrapper.h
// This class manages a single descriptor pool, it updates descriptorsets and resizes the pool when necesarry
// Because of the way descriptorpools work, every wrapper will work for a specific amount of ubos and textures

#ifndef DescriptorPoolWrapperIncluded
#define DescriptorPoolWrapperIncluded

// File includes
#include "VulkanIncludes.h"
#include "Structs.h"

// Standard library includes
#include <vector>

namespace D3D
{
	class Model;

	class DescriptorPoolWrapper final
	{
	public:
		// Constructor
		// Parameters:
		//     uboAmount: the amount of universal buffers per model this pool will contain
		//     textureAmount: the amount of textures per model this pool will contain
		DescriptorPoolWrapper(uint32_t uboAmount, uint32_t textureAmount);

		// Standard descriptor
		~DescriptorPoolWrapper() = default;

		DescriptorPoolWrapper(DescriptorPoolWrapper& other) = delete;
		DescriptorPoolWrapper(DescriptorPoolWrapper&& other) = delete;

		DescriptorPoolWrapper& operator=(DescriptorPoolWrapper& other) = delete;
		DescriptorPoolWrapper& operator=(DescriptorPoolWrapper&& other) = delete;

		// This function will clean up all allocated objects
		// This isn't done in the destructor as the order of objects being deleted is very important in Vulkan
		// Parameters:
		//     device: the vulkan logical device
		void Cleanup(VkDevice device);

		// This function adds a model to the vector of models
		// Parameters:
		//     pModel: the model to add
		void AddModel(Model* pModel);

		// This function removes a model from the vector of models
		// Parameters:
		//     pModel: the model to remove
		void RemoveModel(Model* pModel);

		// This function will create a descriptorset with the given layout
		// Parameters:
		//     layouts: a vector of the Vulkan descriptor set layout that is needed for the creation of descriptor sets
		//     descriptorSets: the vector of descriptor sets that should be created in this function
		void CreateDescriptorSets(std::vector<VkDescriptorSetLayout>& layouts, std::vector<VkDescriptorSet>& descriptorSets);
		
		// This function will update the given descriptorsets
		// Parameters:
		//     descriptorSets: a vector of the vulkan descriptorsets that need to be updated
		//     uboBuffers: a vector of vectors of buffers that will be bound to the descriptorsets
		//     uboSizes: a vector containing the size of the object contained in the uboBuffers
		//     imageViews: a pointer to all the imageViews that will need to be bound to the descriptorsets
		void UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<std::vector<VkBuffer>>& uboBuffers, std::vector<VkDeviceSize>& uboSizes, std::vector<Texture>* textures = nullptr);

	private:
		// The amount of uniform buffer objects per descriptorsets in this pool
		const uint32_t m_UboAmount;

		// The amount of texture objects per descriptorsets in this pool
		const uint32_t m_TextureAmount;

		// The max amount of descriptorsets that can be allocated with this pool
		int m_MaxDescriptorSets{ 8 };

		// The factor with what the pool increases in size when full
		int m_IncreaseFactor{ 2 };

		// The amount of already allocated descriptorsets
		int m_AllocatedDescriptorSets{};

		// The current descriptorpool
		VkDescriptorPool m_DescriptorPool{};
		
		// The models to which the allocated descriptorsets belong, needed when resizing the pool
		std::vector<Model*> m_pModels{};

		// Initialization of the descriptorpool
		void InitDescriptorPool();

		// Resizing of descriptorPool
		void ResizeDescriptorPool();
	};
}

#endif // !DescriptorPoolWrapperIncluded