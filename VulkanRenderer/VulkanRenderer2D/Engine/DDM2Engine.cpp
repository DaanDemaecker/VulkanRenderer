#include "DDM2Engine.h"
#include "Window.h"
#include "Vulkan/Vulkan2D.h"

DDM2::DDM2Engine::DDM2Engine()
{
	DDM2::Window::GetInstance();

	DDM2::Vulkan2D::GetInstance().Init();
}

DDM2::DDM2Engine::~DDM2Engine()
{
}

void DDM2::DDM2Engine::Run()
{
	auto& window{ DDM2::Window::GetInstance() };


	// Variable that will indicate when the gameloop should stop running
	bool shouldQuit{ false };

	// As long as the app shouldn't quit, the gameloop will run
	while (!shouldQuit)
	{
		// Check if aplication should quit
		shouldQuit = glfwWindowShouldClose(window.GetWindowStruct().pWindow);
	}
}
