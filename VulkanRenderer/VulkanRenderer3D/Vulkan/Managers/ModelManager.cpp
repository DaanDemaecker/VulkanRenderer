// ModelManager.cpp

// Header include
#include "ModelManager.h"

// File includes
#include "DataTypes/RenderClasses/Model.h"

DDM3::ModelManager::ModelManager()
{
}

DDM3::ModelManager::~ModelManager()
{
}

void DDM3::ModelManager::Update()
{
	for (auto& pModel : m_pModels)
	{
		pModel->Update();
	}
}

void DDM3::ModelManager::AddModel(std::unique_ptr<Model> pModel)
{
	m_pModels.push_back(std::move(pModel));
}

std::vector<std::unique_ptr<DDM3::Model>>& DDM3::ModelManager::GetModels()
{
	return m_pModels;
}
