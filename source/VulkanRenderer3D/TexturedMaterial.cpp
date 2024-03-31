// TexturedMaterial.cpp

// File includes
#include "TexturedMaterial.h"
#include "VulkanRenderer3D.h"
#include "Utils.h"
#include "DescriptorPoolWrapper.h"
#include "STBIncludes.h"
#include "TextureDescriptorObject.h"

D3D::TexturedMaterial::TexturedMaterial(std::initializer_list<const std::string>&& filePaths, const std::string& pipelineName)
	:Material(pipelineName)
{
	// Set texture amount to the size of the initializer list
	m_TextureAmount = static_cast<int>(filePaths.size());
	
	// Resize textures to textureAmount
	m_Textures.resize(m_TextureAmount);

	// Initialize index variable
	int index{};

	auto& renderer{ VulkanRenderer3D::GetInstance() };

	// Loop trough all filePaths
	for (const auto& path : filePaths)
	{
		// Create texture
		renderer.CreateTexture(m_Textures[index], path);

		// Increment index
		++index;
	}

	m_pDescriptorObject = std::make_unique<D3D::TextureDescriptorObject>(filePaths);

	// Create sampler
	CreateTextureSampler();
}

D3D::TexturedMaterial::~TexturedMaterial()
{
	// Get reference to device for later use
	auto device{ VulkanRenderer3D::GetInstance().GetDevice() };

	// Loop trough textures and destroy them
	for (auto& texture : m_Textures)
	{
		texture.Cleanup(device);
	}
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

	std::vector<DescriptorObject*> descriptorObjectList{};

	for (auto& descriptorObject : descriptorObjects)
	{
		descriptorObjectList.push_back(descriptorObject);
	}

	descriptorObjectList.push_back(VulkanRenderer3D::GetInstance().GetLightDescriptor());

	descriptorObjectList.push_back(m_pDescriptorObject.get());

	// Update descriptorsets
	descriptorPool->UpdateDescriptorSets(descriptorSets, descriptorObjectList);
}

void D3D::TexturedMaterial::CreateTextureSampler()
{
	// Get sampler
	m_TextureSampler = VulkanRenderer3D::GetInstance().GetSampler();
}
