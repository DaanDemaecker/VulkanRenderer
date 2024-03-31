// CubeMapMaterial.cpp

// File includes
#include "CubeMapMaterial.h"
#include "VulkanRenderer3D.h"
#include "DescriptorPoolWrapper.h"
#include "TextureDescriptorObject.h"

// Standard library includes
#include <stdexcept>

D3D::CubeMapMaterial::CubeMapMaterial(const std::initializer_list<const std::string>&& filePaths)
	:CubeMapMaterial(filePaths)
{
	
}

D3D::CubeMapMaterial::CubeMapMaterial(const std::initializer_list<const std::string>& filePaths)
	:Material("Skybox")
{
	// Create the cube texture
	VulkanRenderer3D::GetInstance().CreateCubeTexture(m_CubeTexture, filePaths, m_MipLevels);

	m_pDescriptorObject = std::make_unique<TextureDescriptorObject>(m_CubeTexture);
}

D3D::CubeMapMaterial::~CubeMapMaterial()
{
	// Get reference to device for later use
	auto device{ VulkanRenderer3D::GetInstance().GetDevice() };

	// Clean up the texture
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

void D3D::CubeMapMaterial::UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects)
{
	// Get pointer to the descriptorpool wrapper
	auto descriptorPool = GetDescriptorPool();

	std::vector<DescriptorObject*> descriptorObjectList{};

	for (auto& descriptorObject : descriptorObjects)
	{
		descriptorObjectList.push_back(descriptorObject);
	}

	///descriptorObjectList.push_back(VulkanRenderer3D::GetInstance().GetLightDescriptor());

	descriptorObjectList.push_back(m_pDescriptorObject.get());

	// Update descriptorsets
	descriptorPool->UpdateDescriptorSets(descriptorSets, descriptorObjectList);
}
