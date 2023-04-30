#include "stdafx.h"
#include "Renderer.h"





int main() {

	const uint32_t width = 800;
	const uint32_t height = 600;

	std::unique_ptr<Renderer> pRenderer{ std::make_unique<Renderer>(width, height)};

	while (!pRenderer->WindowShouldClose())
	{
		glfwPollEvents();

		pRenderer->DrawFrame();
	}

	return EXIT_SUCCESS;
}