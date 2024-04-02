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
#include <map>
#include <memory>

namespace D3D
{
	// Class forward declarations
	class Model;
	class DescriptorObject;
	class ShaderModuleWrapper;

	class DescriptorPoolWrapper final
	{
	public:
		// Constructor
		// Parameters:
		//     shaderModules: a vector of shaderModules of the different requested shader files
		DescriptorPoolWrapper(std::vector<std::unique_ptr<D3D::ShaderModuleWrapper>>& shaderModules);

		// Delete default constructor
		DescriptorPoolWrapper() = delete;

		// Standard descriptor
		~DescriptorPoolWrapper() = default;

		// Delete copy and move functions
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
		//     layout: the layout of the descriprot set
		//     descriptorSets: the vector of descriptor sets that should be created in this function
		void CreateDescriptorSets(VkDescriptorSetLayout layouts, std::vector<VkDescriptorSet>& descriptorSets);
		
		// This function will update the given descriptorsets
		// Parameters:
		void UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects);
	private:
		// The amount of bindings per descriptor set type
		std::map<VkDescriptorType, int> m_DescriptorTypeCount{};

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

		// Read the amount of bindings per type from the shader modules
		// Parameters:
		//     shaderModules: a vector of shaderModules of the different requested shader files
		void ReadDescriptorTypeCount(std::vector<std::unique_ptr<D3D::ShaderModuleWrapper>>& shaderModules);
	};
}

#endif // !DescriptorPoolWrapperIncluded