// DDMEngine.cpp

// Header include
#include "DDMEngine.h"

// File includes
#include "ServiceLocator/ServiceLocator.h"
#include "Window/Window.h"

#include "Includes/GLFWIncludes.h"

DDM::DDMEngine::DDMEngine()
{

}

DDM::DDMEngine::~DDMEngine()
{

}

void DDM::DDMEngine::Run()
{
	auto& window{ DDM::Window::GetInstance()};

	DDM::ServiceLocator::GetRenderer().Init();

	// Variable that will indicate when the gameloop should stop running
	bool shouldQuit{ false };

	bool f11PressedLastFrame{ false };

	// As long as the app shouldn't quit, the gameloop will run
	while (!shouldQuit)
	{
		DDM::Window::GetInstance().PollEvents();


		// This is only for testing purposes and will be removed in the future
		//------------
		int state = glfwGetKey(static_cast<GLFWwindow*>(window.GetWindowData().handle), GLFW_KEY_F11);
		if (state == GLFW_PRESS && !f11PressedLastFrame)
		{
			window.ToggleFullscreenMode();
		}
		f11PressedLastFrame = state == GLFW_PRESS;
		//-----------

		// Render
		DDM::ServiceLocator::GetRenderer().Render();

		// Check if aplication should quit
		shouldQuit = window.ShouldClose();
	}

	DDM::ServiceLocator::GetRenderer().Terminate();
}
