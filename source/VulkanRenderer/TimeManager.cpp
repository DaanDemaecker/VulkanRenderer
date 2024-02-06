// TimeManager.cpp

// file includes
#include "TimeManager.h"

void D3D::TimeManager::SetDeltaTime(float dt)
{
	// Set delta time
	m_DeltaTime = dt;
	// Calculate fps
	m_Fps = static_cast<int>(1.0f / dt);
	// Add to total time
	m_TotalTime += dt;
}
