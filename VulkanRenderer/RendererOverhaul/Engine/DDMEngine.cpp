// DDMEngine.cpp

// Header include
#include "DDMEngine.h"

// File includes
#include "Window.h"
#include "ServiceLocator/ServiceLocator.h"

#include "Includes/GLFWIncludes.h"

DDM::DDMEngine::DDMEngine()
{

}

DDM::DDMEngine::~DDMEngine()
{

}

void DDM::DDMEngine::Run()
{
	DDM::ServiceLocator::GetWindow().CreateWindow();

	// Variable that will indicate when the gameloop should stop running
	bool shouldQuit{ false };

	bool f11PressedLastFrame{ false };

	// As long as the app shouldn't quit, the gameloop will run
	while (!shouldQuit)
	{
		int state = glfwGetKey(static_cast<GLFWwindow*>(DDM::ServiceLocator::GetWindow().GetWindowData().handle), GLFW_KEY_F11);
		if (state == GLFW_PRESS && !f11PressedLastFrame)
		{
			DDM::ServiceLocator::GetWindow().ToggleFullscreenMode();
		}
		f11PressedLastFrame = state == GLFW_PRESS;

		DDM::ServiceLocator::GetWindow().PollEvents();

		// Check if aplication should quit
		shouldQuit = DDM::ServiceLocator::GetWindow().ShouldClose();
	}
}
