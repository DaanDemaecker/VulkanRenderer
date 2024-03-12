// TexturedMaterial.cpp

// File includes
#include "TexturedMaterial.h"
#include "VulkanRenderer3D.h"
#include "Utils.h"
#include "DescriptorPoolManager.h"
#include "DescriptorPoolWrapper.h"
#include "STBIncludes.h"

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
		renderer.CreateTexture(m_Textures[index], path, m_MipLevels);

		// Increment index
		++index;
	}

	// Create sampler
	CreateTextureSampler();
}

D3D::TexturedMaterial::~TexturedMaterial()
{
	// Get reference to device for later use
	auto& device{ VulkanRenderer3D::GetInstance().GetDevice() };

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

void D3D::TexturedMaterial::UpdateDescriptorSets(std::vector<VkBuffer>& uboBuffers, std::vector<VkDescriptorSet>& descriptorSets)
{
	// Get descriptorpool associated with this material
	auto descriptorPool = GetDescriptorPool();
	// Create vector of vector of ubo buffers
	std::vector<std::vector<VkBuffer>> uboBuffferList{ uboBuffers, D3D::VulkanRenderer3D::GetInstance().GetLightBuffers() };

	// Create vector for buffersizes
	std::vector<VkDeviceSize> uboSizes(2);
	// Set first size to size of UniformBufferObject
	uboSizes[0] = sizeof(UniformBufferObject);
	// Set second size to size of LightObject
	uboSizes[1] = sizeof(DirectionalLightStruct);

	// Update descriptorsets
	descriptorPool->UpdateDescriptorSets(descriptorSets, uboBuffferList, uboSizes,  &m_Textures);
}

std::vector<VkDescriptorSetLayout>& D3D::TexturedMaterial::GetDescriptorLayout()
{
	// Get descriptorlayout for this material
	// Textured material standardly has 1 veretx ubo, 1 fragment ubo and the amount of textures that was requested
	return VulkanRenderer3D::GetInstance().GetDescriptorSetLayout(1, 1, static_cast<int>(m_Textures.size()));
}

D3D::DescriptorPoolWrapper* D3D::TexturedMaterial::GetDescriptorPool()
{
	// Get descriptorpool for this material
	// Textured material standardly has 2 ubos and the amount of textures that was requested
	return D3D::VulkanRenderer3D::GetInstance().GetDescriptorPoolManager()->GetDescriptorPool(2, m_TextureAmount);
}

void D3D::TexturedMaterial::CreateTextureSampler()
{
	// Get sampler
	m_TextureSampler = VulkanRenderer3D::GetInstance().GetSampler();
}
