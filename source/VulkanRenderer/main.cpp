#include "stdafx.h"
#include "Application.h"

#include "D3DEngine.h"
#include "VulkanRenderer.h"

//#define OLD
#define NEW

void load()
{
	//auto& renderer{ D3D::VulkanRenderer::GetInstance() };
}


int main()
{
	const uint32_t windowWidth = 800;
	const uint32_t windowHeight = 600;

#ifdef NEW
	D3D::D3DEngine engine(static_cast<int>(windowWidth), static_cast<int>(windowHeight));
	engine.Run(load);

#else ifdef OLD
	std::unique_ptr<Application> pRenderer{ std::make_unique<Application>(windowWidth, windowHeight)};

	while (!pRenderer->WindowShouldClose())
	{
		glfwPollEvents();

		pRenderer->DrawFrame();
	}
#endif

	return EXIT_SUCCESS;
} 