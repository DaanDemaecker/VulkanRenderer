// CameraManager

#ifndef CameraManagerIncluded
#define CameraManagerIncluded

// Standard library includes
#include <memory>

namespace D3D
{
	class Camera;

	class CameraManager final
	{
	public:
		CameraManager();
		~CameraManager();

		void SetCamera(std::unique_ptr<Camera> pNewCamera);

		Camera* GetCamera();

	private:
		std::unique_ptr<Camera> m_pCurrentCamera{};

		std::unique_ptr<Camera> m_pDefaultCamera{};
	};
}
#endif // !CameraManagerIncluded
