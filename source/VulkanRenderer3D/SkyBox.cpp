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
	auto pMaterial = std::make_shared<CubeMapMaterial>(filePaths);

	m_pModel = std::make_unique<Model>();
	m_pModel->LoadModel("../Resources/Models/cube.obj");
	m_pModel->SetMaterial(pMaterial);
}

void D3D::SkyBox::Render()
{
	m_pModel->Render();
}
