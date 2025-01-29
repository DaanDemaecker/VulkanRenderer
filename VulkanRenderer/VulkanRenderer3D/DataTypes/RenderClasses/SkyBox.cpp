// SkyBox.cpp

// Header include
#include "SkyBox.h"

// File includes
#include "Utils/Utils.h"

#include "Vulkan/Renderers/VulkanRenderer3D.h"

#include "Model.h"

#include "DataTypes/Materials/CubeMapMaterial.h"
#include "DataTypes/DescriptorObjects/TextureDescriptorObject.h"

// Standard library includes

DDM3::SkyBox::SkyBox(std::initializer_list<const std::string>&& filePaths)
{
	// Create cubemap material
	auto pMaterial = std::make_shared<CubeMapMaterial>(filePaths);

	// Create the model
	m_pModel = std::make_unique<Model>();

	// Load a cube model
	m_pModel->LoadModel("Resources/Models/cube.obj");

	// Set the material
	m_pModel->SetMaterial(pMaterial);
}

DDM3::SkyBox::~SkyBox()
{
}

void DDM3::SkyBox::Render()
{
	// Render the model
	m_pModel->Render();
}
