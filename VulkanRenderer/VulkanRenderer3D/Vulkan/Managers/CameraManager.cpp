// CameraManager.cpp

// Header include
#include "CameraManager.h"

// File includes
#include "DataTypes/Camera.h"

DDM3::CameraManager::CameraManager()
{
	m_pDefaultCamera = std::make_unique<Camera>();
}

DDM3::CameraManager::~CameraManager()
{
}

void DDM3::CameraManager::SetCamera(std::unique_ptr<Camera> pNewCamera)
{
	if (pNewCamera != nullptr)
	{
		m_pCurrentCamera = std::move(pNewCamera);
	}
}

DDM3::Camera* DDM3::CameraManager::GetCamera()
{
	if (m_pCurrentCamera != nullptr)
	{
		return m_pCurrentCamera.get();
	}

	return m_pDefaultCamera.get();
}

void DDM3::CameraManager::SetSkybox(std::unique_ptr<SkyBox> pSkybox)
{
	m_pSkyBox = std::move(pSkybox);
}

void DDM3::CameraManager::RenderSkybox()
{
	if (m_pSkyBox != nullptr && GetCamera()->GetCameraType() == CameraType::Perspective)
	{
		m_pSkyBox->Render();
	}
}
