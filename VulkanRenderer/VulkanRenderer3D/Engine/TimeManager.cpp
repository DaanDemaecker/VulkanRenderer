// TimeManager.cpp

// Header include
#include "TimeManager.h"

void DDM3::TimeManager::SetDeltaTime(float dt)
{
	// Set delta time
	m_DeltaTime = dt;
	// Calculate fps
	m_Fps = static_cast<int>(1.0f / dt);
	// Add to total time
	m_TotalTime += dt;
}
