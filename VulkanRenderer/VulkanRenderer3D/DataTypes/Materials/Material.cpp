// Material.cpp

// Header file include
#include "Material.h"

// File includes
#include "Includes/STBIncludes.h"

#include "Utils/Utils.h"

#include "Vulkan/Renderers/VulkanRenderer3D.h"
#include "Vulkan/Wrappers/DescriptorPoolWrapper.h"
#include "Vulkan/Wrappers/PipelineWrapper.h"

#include "DataTypes/DescriptorObjects/DescriptorObject.h"
#include "DataTypes/DirectionalLightObject.h"

DDM3::Material::Material(const std::string& pipelineName)
{
	// Get the requested pipeline from the renderer
	m_Pipeline = Vulkan3D::GetInstance().GetRenderer().GetPipeline(pipelineName);
}

DDM3::Material::~Material()
{
}

DDM3::PipelineWrapper* DDM3::Material::GetPipeline()
{
	// Return the pipeline
	return m_Pipeline;
}

void DDM3::Material::CreateDescriptorSets(Model* pModel, std::vector<VkDescriptorSet>& descriptorSets)
{
	// Get pointer to the descriptorpool wrapper
	auto descriptorPool = GetDescriptorPool();
	// Add model to descriptorpool wrapper
	descriptorPool->AddModel(pModel);
	// Create descriptorpools
	descriptorPool->CreateDescriptorSets(GetDescriptorLayout(), descriptorSets);
}

void DDM3::Material::UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects)
{
	// Get pointer to the descriptorpool wrapper
	auto descriptorPool = GetDescriptorPool();

	// Create list of descriptor objects and add the objects of the model to it
	std::vector<DescriptorObject*> descriptorObjectList{};

	for (auto& descriptorObject : descriptorObjects)
	{
		descriptorObjectList.push_back(descriptorObject);
	}

	// Add the descriptor of the global light object
	descriptorObjectList.push_back(Vulkan3D::GetInstance().GetRenderer().GetGlobalLight()->GetDescriptorObject());

	// Update descriptorsets
	descriptorPool->UpdateDescriptorSets(descriptorSets, descriptorObjectList);
}

VkDescriptorSetLayout DDM3::Material::GetDescriptorLayout()
{
	// Get the descriptorsets layout
	return m_Pipeline->GetDescriptorSetLayout();
}

DDM3::DescriptorPoolWrapper* DDM3::Material::GetDescriptorPool()
{
	// Get descriptorpoolwrapper
	return m_Pipeline->GetDescriptorPool();
}
