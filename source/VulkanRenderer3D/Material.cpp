// Material.cpp

// File includes
#include "Material.h"
#include "VulkanRenderer3D.h"
#include "Utils.h"
#include "DescriptorPoolWrapper.h"
#include "STBIncludes.h"
#include "PipelineWrapper.h"
#include "DescriptorObject.h"

D3D::Material::Material(const std::string& pipelineName)
{
	// Get the requested pipeline from the renderer
	m_Pipeline = VulkanRenderer3D::GetInstance().GetPipeline(pipelineName);
}

D3D::PipelineWrapper* D3D::Material::GetPipeline()
{
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

	std::vector<DescriptorObject*> descriptorObjectList{};

	for (auto& descriptorObject : descriptorObjects)
	{
		descriptorObjectList.push_back(descriptorObject);
	}

	descriptorObjectList.push_back(VulkanRenderer3D::GetInstance().GetLightDescriptor());

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
