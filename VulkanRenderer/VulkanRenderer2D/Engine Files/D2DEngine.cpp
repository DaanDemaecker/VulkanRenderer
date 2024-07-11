#include "D2DEngine.h"
#include "Window.h"
#include "../Vulkan/VulkanRenderer2D.h"

D2D::D2DEngine::D2DEngine()
{
	D2D::Window::GetInstance();
	D2D::VulkanRenderer2D::GetInstance();
}

D2D::D2DEngine::~D2DEngine()
{
}

void D2D::D2DEngine::Run()
{
	auto& window{ D2D::Window::GetInstance() };


	// Variable that will indicate when the gameloop should stop running
	bool shouldQuit{ false };

	// As long as the app shouldn't quit, the gameloop will run
	while (!shouldQuit)
	{// Check if aplication should quit
		shouldQuit = glfwWindowShouldClose(window.GetWindowStruct().pWindow);
	}
}
