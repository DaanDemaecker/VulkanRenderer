// Material.cpp

// File includes
#include "Material.h"
#include "VulkanRenderer.h"
#include "Utils.h"
#include "DescriptorPoolWrapper.h"
#include "DescriptorPoolManager.h"
#include "STBIncludes.h"

D3D::Material::Material(const std::string& pipelineName)
{
	// Get the requested pipeline from the renderer
	m_PipelinePair = VulkanRenderer::GetInstance().GetPipeline(pipelineName);
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
	std::vector<std::vector<VkBuffer>> uboList{ uboBuffers, D3D::VulkanRenderer::GetInstance().GetLightBuffers()};

	// Create vector for buffersizes
	std::vector<VkDeviceSize> uboSizes(2);
	// Set first size to size of UniformBufferObject
	uboSizes[0] = sizeof(UniformBufferObject);
	// Set second size to size of LightObject
	uboSizes[1] = sizeof(LightObject);

	// Update descriptorsets
	descriptorPool->UpdateDescriptorSets(descriptorSets, uboList, uboSizes);
}

std::vector<VkDescriptorSetLayout>& D3D::Material::GetDescriptorLayout()
{
	// Get the descriptorsets layout
	// Standard material has 1 vertshader ubo, 1 fragshader ubo and no textures
	return VulkanRenderer::GetInstance().GetDescriptorSetLayout(1, 1, 0);
}

D3D::DescriptorPoolWrapper* D3D::Material::GetDescriptorPool()
{
	// Get descriptorpoolwrapper
	// Standard material has 2 ubos and 0 textures
	return D3D::VulkanRenderer::GetInstance().GetDescriptorPoolManager()->GetDescriptorPool(2, 0);
}
