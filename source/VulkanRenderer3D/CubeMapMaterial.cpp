// CubeMapMaterial.cpp

// File includes
#include "CubeMapMaterial.h"
#include "VulkanRenderer3D.h"
#include "DescriptorPoolManager.h"
#include "DescriptorPoolWrapper.h"

// Standard library includes
#include <stdexcept>

D3D::CubeMapMaterial::CubeMapMaterial(const std::initializer_list<const std::string>&& filePaths)
	:CubeMapMaterial(filePaths)
{
	
}

D3D::CubeMapMaterial::CubeMapMaterial(const std::initializer_list<const std::string>& filePaths)
	:Material("Skybox")
{
	if (filePaths.size() < 6)
	{
		throw std::runtime_error("6 or more images are required for a cube map");
	}

	VulkanRenderer3D::GetInstance().CreateCubeTexture(m_CubeTexture, filePaths, m_MipLevels);
}

D3D::CubeMapMaterial::~CubeMapMaterial()
{
	// Get reference to device for later use
	auto& device{ VulkanRenderer3D::GetInstance().GetDevice() };

	m_CubeTexture.Cleanup(device);
}

void D3D::CubeMapMaterial::CreateDescriptorSets(Model* pModel, std::vector<VkDescriptorSet>& descriptorSets)
{
	// Get descriptorpool associated with this material
	auto descriptorPool = GetDescriptorPool();
	// Add model to descriptorpool
	descriptorPool->AddModel(pModel);
	// Create descriptorpool
	descriptorPool->CreateDescriptorSets(GetDescriptorLayout(), descriptorSets);
}

void D3D::CubeMapMaterial::UpdateDescriptorSets(std::vector<VkBuffer>& uboBuffers, std::vector<VkDescriptorSet>& descriptorSets)
{
	// Get descriptorpool associated with this material
	auto descriptorPool = GetDescriptorPool();
	// Create vector of vector of ubo buffers
	std::vector<std::vector<VkBuffer>> uboBuffferList{ uboBuffers};

	// Create vector for buffersizes
	std::vector<VkDeviceSize> uboSizes(1);
	// Set first size to size of UniformBufferObject
	uboSizes[0] = sizeof(UniformBufferObject);

	auto texture = std::vector<Texture>{ m_CubeTexture };

	// Update descriptorsets
	descriptorPool->UpdateDescriptorSets(descriptorSets, uboBuffferList, uboSizes, &texture);
}

std::vector<VkDescriptorSetLayout>& D3D::CubeMapMaterial::GetDescriptorLayout()
{
	// Get descriptorlayout for this material
	// CubeMapMaterial material standardly has 1 vertex ubo, 0 fragment ubos and 1 texture
	return VulkanRenderer3D::GetInstance().GetDescriptorSetLayout(1,0,1);
}

D3D::DescriptorPoolWrapper* D3D::CubeMapMaterial::GetDescriptorPool()
{
	// Get descriptorpool for this material
	// CubeMapMaterial material standardly has 1 vertex ubo, 0 fragment ubos and 1 texture
	return D3D::VulkanRenderer3D::GetInstance().GetDescriptorPoolManager()->GetDescriptorPool(1, 1);
}
