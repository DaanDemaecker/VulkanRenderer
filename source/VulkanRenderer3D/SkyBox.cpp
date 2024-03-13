// SkyBox.cpp

// File includes
#include "SkyBox.h"
#include "Utils.h"
#include "VulkanRenderer3D.h"
#include "Model.h"
#include "CubeMapMaterial.h"

// Standard library includes
#include <string>

D3D::SkyBox::SkyBox(std::initializer_list<const std::string>&& filePaths)
{
	// Create cubemap material
	auto pMaterial = std::make_shared<CubeMapMaterial>(filePaths);

	// Create the model
	m_pModel = std::make_unique<Model>();

	// Load a cube model
	m_pModel->LoadModel("../Resources/Models/cube.obj");

	// Set the material
	m_pModel->SetMaterial(pMaterial);
}

void D3D::SkyBox::Render()
{
	// Render the model
	m_pModel->Render();
}
