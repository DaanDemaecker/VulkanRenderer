// ShadowMaterial.cpp

// File includes
#include "ShadowMaterial.h"
#include "VulkanRenderer3D.h"
#include "Utils.h"
#include "DescriptorPoolWrapper.h"
#include "STBIncludes.h"
#include "TextureDescriptorObject.h"
#include "DirectionalLightObject.h"

D3D::ShadowMaterial::ShadowMaterial(std::initializer_list<const std::string>&& filePaths, const std::string& pipelineName)
	:Material(pipelineName)
{
	// Create a descriptor object with the list of file paths given
	m_pDescriptorObject = std::make_unique<D3D::TextureDescriptorObject>(filePaths);

	// Create sampler
	CreateTextureSampler();
}

void D3D::ShadowMaterial::CreateDescriptorSets(Model* pModel, std::vector<VkDescriptorSet>& descriptorSets)
{
	// Get descriptorpool associated with this material
	auto descriptorPool = GetDescriptorPool();
	// Add model to descriptorpool
	descriptorPool->AddModel(pModel);
	// Create descriptorpool
	descriptorPool->CreateDescriptorSets(GetDescriptorLayout(), descriptorSets);
}

void D3D::ShadowMaterial::UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects)
{
	// Get pointer to the descriptorpool wrapper
	auto descriptorPool = GetDescriptorPool();

	// Create list of descriptor objects and add the objects of the model to it
	std::vector<DescriptorObject*> descriptorObjectList{};

	for (auto& descriptorObject : descriptorObjects)
	{
		descriptorObjectList.push_back(descriptorObject);
	}



	// Add the descriptor object of the global light
	descriptorObjectList.push_back(VulkanRenderer3D::GetInstance().GetGlobalLight()->GetTransformDescriptorObject());

	// Add the descriptor object of the global light
	descriptorObjectList.push_back(VulkanRenderer3D::GetInstance().GetGlobalLight()->GetDescriptorObject());

	// Add the descriptor object holding the textures
	descriptorObjectList.push_back(m_pDescriptorObject.get());

	// Add the descriptor object of the global light
	descriptorObjectList.push_back(VulkanRenderer3D::GetInstance().GetShadowMapDescriptorObject());

	// Update descriptorsets
	descriptorPool->UpdateDescriptorSets(descriptorSets, descriptorObjectList);
}

void D3D::ShadowMaterial::CreateTextureSampler()
{
	// Get sampler
	m_TextureSampler = VulkanRenderer3D::GetInstance().GetSampler();
}
