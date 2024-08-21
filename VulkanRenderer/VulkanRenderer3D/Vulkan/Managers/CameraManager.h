// CameraManager

#ifndef CameraManagerIncluded
#define CameraManagerIncluded

// File includes
#include "DataTypes/RenderClasses/SkyBox.h"

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

		void SetSkybox(std::unique_ptr<SkyBox> pSkybox);

		void RenderSkybox();

	private:
		std::unique_ptr<Camera> m_pCurrentCamera{};

		std::unique_ptr<Camera> m_pDefaultCamera{};

		// Pointer to the skybox object
		std::unique_ptr<SkyBox> m_pSkyBox{};
	};
}
#endif // !CameraManagerIncluded
