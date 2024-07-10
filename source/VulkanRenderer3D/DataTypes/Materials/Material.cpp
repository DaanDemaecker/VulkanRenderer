// Material.cpp

// File includes
#include "Material.h"
#include "../../Vulkan/Renderers/VulkanRenderer3D.h"
#include "../../Utils/Utils.h"
#include "../../Vulkan/Wrappers/DescriptorPoolWrapper.h"
#include "../../Includes/STBIncludes.h"
#include "../../Vulkan/Wrappers/PipelineWrapper.h"
#include "../../DataTypes/DescriptorObjects/DescriptorObject.h"
#include "../../DataTypes/DirectionalLightObject.h"

D3D::Material::Material(const std::string& pipelineName)
{
	// Get the requested pipeline from the renderer
	m_Pipeline = VulkanRenderer3D::GetInstance().GetPipeline(pipelineName);
}

D3D::PipelineWrapper* D3D::Material::GetPipeline()
{
	// Return the pipeline
	return m_Pipeline;
}

void D3D::Material::CreateDescriptorSets(Model* pModel, std::vector<VkDescriptorSet>& descriptorSets)
{
	// Get pointer to the descriptorpool wrapper
	auto descriptorPool = GetDescriptorPool();
	// Add model to descriptorpool wrapper
	descriptorPool->AddModel(pModel);
	// Create descriptorpools
	descriptorPool->CreateDescriptorSets(GetDescriptorLayout(), descriptorSets);
}

void D3D::Material::UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects)
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
	descriptorObjectList.push_back(VulkanRenderer3D::GetInstance().GetGlobalLight()->GetDescriptorObject());

	// Update descriptorsets
	descriptorPool->UpdateDescriptorSets(descriptorSets, descriptorObjectList);
}

VkDescriptorSetLayout D3D::Material::GetDescriptorLayout()
{
	// Get the descriptorsets layout
	return m_Pipeline->GetDescriptorSetLayout();
}

D3D::DescriptorPoolWrapper* D3D::Material::GetDescriptorPool()
{
	// Get descriptorpoolwrapper
	return m_Pipeline->GetDescriptorPool();
}
