#include "DDMEngine.h"
#include "Window.h"

DDM::DDMEngine::DDMEngine()
{
	DDM::Window::GetInstance();
}

DDM::DDMEngine::~DDMEngine()
{

}

void DDM::DDMEngine::Run()
{
	auto& window{ DDM::Window::GetInstance() };


	// Variable that will indicate when the gameloop should stop running
	bool shouldQuit{ false };

	// As long as the app shouldn't quit, the gameloop will run
	while (!shouldQuit)
	{


		glfwPollEvents();

		// Check if aplication should quit
		shouldQuit = glfwWindowShouldClose(window.GetWindowStruct().pWindow);
	}
}
