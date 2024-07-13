// TexturedMaterial.cpp

// File includes
#include "TexturedMaterial.h"
#include "Vulkan/Renderers/VulkanRenderer3D.h"
#include "Utils/Utils.h"
#include "Vulkan/Wrappers/DescriptorPoolWrapper.h"
#include "Includes/STBIncludes.h"
#include "DataTypes/DescriptorObjects/TextureDescriptorObject.h"
#include "DataTypes/DirectionalLightObject.h"

D3D::TexturedMaterial::TexturedMaterial(std::initializer_list<const std::string>&& filePaths, const std::string& pipelineName)
	:Material(pipelineName)
{
	// Create a descriptor object with the list of file paths given
	m_pDescriptorObject = std::make_unique<D3D::TextureDescriptorObject>(filePaths);

	// Create sampler
	CreateTextureSampler();
}

void D3D::TexturedMaterial::CreateDescriptorSets(Model* pModel, std::vector<VkDescriptorSet>& descriptorSets)
{
	// Get descriptorpool associated with this material
	auto descriptorPool = GetDescriptorPool();
	// Add model to descriptorpool
	descriptorPool->AddModel(pModel);
	// Create descriptorpool
	descriptorPool->CreateDescriptorSets(GetDescriptorLayout(), descriptorSets);
}

void D3D::TexturedMaterial::UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects)
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
	descriptorObjectList.push_back(VulkanRenderer3D::GetInstance().GetGlobalLight()->GetDescriptorObject());

	// Add the descriptor object holding the textures
	descriptorObjectList.push_back(m_pDescriptorObject.get());

	// Update descriptorsets
	descriptorPool->UpdateDescriptorSets(descriptorSets, descriptorObjectList);
}

void D3D::TexturedMaterial::CreateTextureSampler()
{
	// Get sampler
	m_TextureSampler = VulkanRenderer3D::GetInstance().GetSampler();
}
