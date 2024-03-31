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
	Texture cubeTexture{};

	// Create the cube texture
	VulkanRenderer3D::GetInstance().CreateCubeTexture(cubeTexture, filePaths, m_MipLevels);

	m_pDescriptorObject = std::make_unique<TextureDescriptorObject>(cubeTexture);
}

D3D::CubeMapMaterial::~CubeMapMaterial()
{

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
