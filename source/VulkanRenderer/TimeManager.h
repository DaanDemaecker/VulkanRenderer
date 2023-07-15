#ifndef TimeManagerIncluded
#define TimeManagerIncluded

#include "Singleton.h"

namespace D3D
{
	class TimeManager final : public Singleton<TimeManager>
	{
	public:
		void SetDeltaTime(float dt);
		float GetDeltaTime() const { return m_DeltaTime; }

		int GetFps() const { return m_Fps; }

		void SetFixedTime(float ft) { m_FixedTime = ft; }
		float GetFixedTime() const { return m_FixedTime; }

	private:
		int m_Fps = 0;
		float m_DeltaTime = 0;
		float m_FixedTime = 0;
	};
}


#endif // !TimeManagerIncluded