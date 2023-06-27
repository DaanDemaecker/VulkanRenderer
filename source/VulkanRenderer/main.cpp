#include "stdafx.h"
#include "Application.h"

#include "D3DEngine.h"

#include "ServiceLocator.h"
#include "VulkanRenderer.h"

void load()
{
	D3D::ServiceLocator::RegisterRenderer(std::make_unique<D3D::VulkanRenderer>());
}


int main()
{
	const uint32_t width = 800;
	const uint32_t height = 600;

	D3D::D3DEngine engine(static_cast<int>(width), static_cast<int>(height));
	engine.Run(load);

	/*std::unique_ptr<Application> pRenderer{ std::make_unique<Application>(width, height)};

	while (!pRenderer->WindowShouldClose())
	{
		glfwPollEvents();

		pRenderer->DrawFrame();
	}*/

	return EXIT_SUCCESS;
}