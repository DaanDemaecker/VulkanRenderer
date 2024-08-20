// CameraManager.cpp

// Header include
#include "CameraManager.h"

// File includes
#include "DataTypes/Camera.h"

D3D::CameraManager::CameraManager()
{
	m_pDefaultCamera = std::make_unique<Camera>();
}

D3D::CameraManager::~CameraManager()
{
}

void D3D::CameraManager::SetCamera(std::unique_ptr<Camera> pNewCamera)
{
	if (pNewCamera != nullptr)
	{
		m_pCurrentCamera = std::move(pNewCamera);
	}
}

D3D::Camera* D3D::CameraManager::GetCamera()
{
	if (m_pCurrentCamera != nullptr)
	{
		return m_pCurrentCamera.get();
	}

	return m_pDefaultCamera.get();
}
