// DDMEngine.cpp

// Header include
#include "DDMEngine.h"

// File includes
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
		DDM::ServiceLocator::GetWindow().PollEvents();


		// This is only for testing purposes and will be removed in the future
		//------------
		int state = glfwGetKey(static_cast<GLFWwindow*>(DDM::ServiceLocator::GetWindow().GetWindowData().handle), GLFW_KEY_F11);
		if (state == GLFW_PRESS && !f11PressedLastFrame)
		{
			DDM::ServiceLocator::GetWindow().ToggleFullscreenMode();
		}
		f11PressedLastFrame = state == GLFW_PRESS;
		//-----------

		// Check if aplication should quit
		shouldQuit = DDM::ServiceLocator::GetWindow().ShouldClose();
	}
}
