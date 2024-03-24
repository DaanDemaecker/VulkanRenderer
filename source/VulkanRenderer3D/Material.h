// Material.h
// This is the base class for all the materials
// Materials are used to bind a graphics pipeline and/or a texture to a model

#ifndef MaterialIncluded
#define MaterialIncluded

// File includes
#include "Structs.h"

// Standard library includes
#include <iostream>

namespace D3D
{
	// Class forward declarations
	class DescriptorPoolWrapper;
	class Model;
	class PipelineWrapper;

	class Material
	{
	public:
		// Constructor
		// Parameters:
		//     pipelineName: the name of the graphicspipeline this material will use, standard set to "Default"
		Material(const std::string& pipelineName = "Default");
		// Default constructor
		virtual ~Material() = default;

		Material(Material& other) = delete;
		Material(Material&& other) = delete;

		Material& operator=(Material& other) = delete;
		Material& operator=(Material&& other) = delete; 

		// Get the pipeline that is used by this material
		PipelineWrapper* GetPipeline();

		// Create the descriptorsets
		// Parameters:
		//     pModel: the model that the descriptorsets belong to
		//     descriptorSets: vector of descriptorsets that have to be created
		virtual void CreateDescriptorSets(Model* pModel, std::vector<VkDescriptorSet>& descriptorSets);

		// Update the descriptorsets
		// Parameters:
		//     uboBuffers: vector of the buffers for Uniform Buffer Objects
		//     descriptorsets: the descriptorsets that should be updated
		virtual void UpdateDescriptorSets(std::vector<VkBuffer>& uboBuffers, std::vector<VkDescriptorSet>& descriptorSets);

		// Get the descriptor set layout
		VkDescriptorSetLayout GetDescriptorLayout();

		// Get the descriptorpool wrapper
		DescriptorPoolWrapper* GetDescriptorPool();

	protected:
		// The pipeline pair that is used for this material
		PipelineWrapper* m_Pipeline{};
	};
}
#endif // !MaterialIncluded