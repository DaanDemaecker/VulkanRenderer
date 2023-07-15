#include "stdafx.h"
#include "TimeManager.h"

void D3D::TimeManager::SetDeltaTime(float dt)
{
	m_DeltaTime = dt;
	m_Fps = static_cast<int>(1.0f / dt);
}
