// ShadowMaterial.cpp

// Header include
#include "ShadowMaterial.h"

// File includes
#include "Includes/STBIncludes.h"

#include "Utils/Utils.h"

#include "Vulkan/Vulkan3D.h"
#include "Vulkan/Wrappers/DescriptorPoolWrapper.h"

#include "DataTypes/DescriptorObjects/TextureDescriptorObject.h"
#include "DataTypes/DirectionalLightObject.h"

DDM3::ShadowMaterial::ShadowMaterial(std::initializer_list<const std::string>&& filePaths, const std::string& pipelineName)
	:Material(pipelineName)
{
	// Create a descriptor object with the list of file paths given
	m_pDescriptorObject = std::make_unique<DDM3::TextureDescriptorObject>(filePaths);

	// Create sampler
	CreateTextureSampler();
}

DDM3::ShadowMaterial::~ShadowMaterial()
{
}

void DDM3::ShadowMaterial::CreateDescriptorSets(Model* pModel, std::vector<VkDescriptorSet>& descriptorSets)
{
	// Get descriptorpool associated with this material
	auto descriptorPool = GetDescriptorPool();
	// Add model to descriptorpool
	descriptorPool->AddModel(pModel);
	// Create descriptorpool
	descriptorPool->CreateDescriptorSets(GetDescriptorLayout(), descriptorSets);
}

void DDM3::ShadowMaterial::UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects)
{
	// Get pointer to the descriptorpool wrapper
	auto descriptorPool = GetDescriptorPool();

	// Create list of descriptor objects and add the objects of the model to it
	std::vector<DescriptorObject*> descriptorObjectList{};

	for (auto& descriptorObject : descriptorObjects)
	{
		descriptorObjectList.push_back(descriptorObject);
	}

	auto& renderer{ Vulkan3D::GetInstance().GetRenderer() };


	// Add the descriptor object of the global light
	descriptorObjectList.push_back(renderer.GetGlobalLight()->GetTransformDescriptorObject());

	// Add the descriptor object of the global light
	descriptorObjectList.push_back(renderer.GetGlobalLight()->GetDescriptorObject());

	// Add the descriptor object holding the textures
	descriptorObjectList.push_back(m_pDescriptorObject.get());

	// Add the descriptor object of the global light
	descriptorObjectList.push_back(renderer.GetShadowMapDescriptorObject());

	// Update descriptorsets
	descriptorPool->UpdateDescriptorSets(descriptorSets, descriptorObjectList);
}

void DDM3::ShadowMaterial::CreateTextureSampler()
{
	// Get sampler
	m_TextureSampler = Vulkan3D::GetInstance().GetRenderer().GetSampler();
}
