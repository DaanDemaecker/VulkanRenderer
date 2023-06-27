#include "stdafx.h"
#include "Application.h"

int main() {

	const uint32_t width = 800;
	const uint32_t height = 600;

	std::unique_ptr<Application> pRenderer{ std::make_unique<Application>(width, height)};

	while (!pRenderer->WindowShouldClose())
	{
		glfwPollEvents();

		pRenderer->DrawFrame();
	}

	return EXIT_SUCCESS;
}