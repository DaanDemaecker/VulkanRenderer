// ModelManager.cpp

// Header include
#include "ModelManager.h"

// File includes
#include "DataTypes/RenderClasses/Model.h"

D3D::ModelManager::ModelManager()
{
}

D3D::ModelManager::~ModelManager()
{
}

void D3D::ModelManager::Update()
{
	for (auto& pModel : m_pModels)
	{
		pModel->Update();
	}
}

void D3D::ModelManager::AddModel(std::unique_ptr<Model> pModel)
{
	m_pModels.push_back(std::move(pModel));
}

std::vector<std::unique_ptr<D3D::Model>>& D3D::ModelManager::GetModels()
{
	return m_pModels;
}
