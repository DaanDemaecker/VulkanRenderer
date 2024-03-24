// Material.cpp

// File includes
#include "Material.h"
#include "VulkanRenderer3D.h"
#include "Utils.h"
#include "DescriptorPoolWrapper.h"
#include "DescriptorPoolManager.h"
#include "STBIncludes.h"
#include "PipelineWrapper.h"

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

void D3D::Material::UpdateDescriptorSets(std::vector<VkBuffer>& uboBuffers, std::vector<VkDescriptorSet>& descriptorSets)
{
	// Get pointer to the descriptorpool wrapper
	auto descriptorPool = GetDescriptorPool();
	// Create vector of vectors of vkBuffers
	std::vector<std::vector<VkBuffer>> uboList{ uboBuffers, D3D::VulkanRenderer3D::GetInstance().GetLightBuffers()};

	// Create vector for buffersizes
	std::vector<VkDeviceSize> uboSizes(2);
	// Set first size to size of UniformBufferObject
	uboSizes[0] = sizeof(UniformBufferObject);
	// Set second size to size of LightObject
	uboSizes[1] = sizeof(DirectionalLightStruct);

	// Update descriptorsets
	descriptorPool->UpdateDescriptorSets(descriptorSets, uboList, uboSizes);
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
