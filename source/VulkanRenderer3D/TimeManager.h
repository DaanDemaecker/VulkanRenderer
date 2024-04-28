// TimeManager.h
// This class will manage all things time related such as deltaTime, fps etc.

#ifndef TimeManagerIncluded
#define TimeManagerIncluded

// File includes
#include "Singleton.h"

namespace D3D
{
	class TimeManager final : public Singleton<TimeManager>
	{
	public:
		// Set the delta time for this frame
		// Parameters:
		//     dt: delta time in seconds
		void SetDeltaTime(float dt);
		// Get delta time for this frame
		float GetDeltaTime() const { return m_DeltaTime; }

		// Get current fps
		int GetFps() const { return m_Fps; }

		// Get total time the app has been running
		float GetTotalTime() const { return m_TotalTime; }

	private:
		// The current amount of frames per second
		int m_Fps = 0;
		// The delta time for the current frame
		float m_DeltaTime = 0;
		// Total time the app has been running
		float m_TotalTime = 0;
	};
}


#endif // !TimeManagerIncluded