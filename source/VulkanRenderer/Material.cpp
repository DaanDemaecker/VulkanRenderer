#include "Material.h"
#include "VulkanRenderer.h"
#include "Utils.h"

#include "DescriptorPoolWrapper.h"
#include "DescriptorPoolManager.h"

#include "STBIncludes.h"

D3D::Material::Material(const std::string& pipelineName)
{
	m_PipelinePair = VulkanRenderer::GetInstance().GetPipeline(pipelineName);
}

void D3D::Material::CreateDescriptorSets(Model* pModel, std::vector<VkDescriptorSet>& descriptorSets)
{
	auto descriptorPool = GetDescriptorPool();
	descriptorPool->AddModel(pModel);
	descriptorPool->CreateDescriptorSets(*GetDescriptorLayout(), descriptorSets);
}

void D3D::Material::UpdateDescriptorSets(std::vector<VkBuffer>& uboBuffers, std::vector<VkDescriptorSet>& descriptorSets)
{
	auto descriptorPool = GetDescriptorPool();
	std::vector<std::vector<VkBuffer>> uboList{ uboBuffers, D3D::VulkanRenderer::GetInstance().GetLightBuffers()};

	std::vector<VkDeviceSize> uboSizes(2);
	uboSizes[0] = sizeof(UniformBufferObject);
	uboSizes[1] = sizeof(LightObject);

	descriptorPool->UpdateDescriptorSets(uboList, uboSizes, descriptorSets);
}

VkDescriptorSetLayout* D3D::Material::GetDescriptorLayout()
{
	return VulkanRenderer::GetInstance().GetDescriptorSetLayout(1, 1, 0);
}

D3D::DescriptorPoolWrapper* D3D::Material::GetDescriptorPool()
{
	return D3D::VulkanRenderer::GetInstance().GetDescriptorPoolManager()->GetDescriptorPool(2, 0);
}
