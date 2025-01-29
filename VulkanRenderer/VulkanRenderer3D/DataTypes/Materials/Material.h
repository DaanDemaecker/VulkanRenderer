// Material.h
// This is the base class for all the materials
// Materials are used to bind a graphics pipeline and/or a texture to a model

#ifndef MaterialIncluded
#define MaterialIncluded

// File includes
#include "DataTypes/Structs.h"

// Standard library includes
#include <iostream>

namespace DDM3
{
	// Class forward declarations
	class DescriptorPoolWrapper;
	class Model;
	class PipelineWrapper;
	class DescriptorObject;

	class Material
	{
	public:
		// Constructor
		// Parameters:
		//     pipelineName: the name of the graphicspipeline this material will use, standard set to "Default"
		Material(const std::string& pipelineName = "Default");
		// Default constructor
		virtual ~Material();

		// Delete copy and move functions
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
		//     descriptorsets: the descriptorsets that should be updated
		//     descriptorObjects: a vector of pointers to descriptorobjects in the same order as the shader code
		virtual void UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects);

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