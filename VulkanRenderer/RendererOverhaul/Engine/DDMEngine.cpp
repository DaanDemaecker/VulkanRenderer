// DDMEngine.cpp

// Header include
#include "DDMEngine.h"

// File includes
#include "Window.h"
#include "ServiceLocator/ServiceLocator.h"

DDM::DDMEngine::DDMEngine()
{
	//DDM::Window::GetInstance();
}

DDM::DDMEngine::~DDMEngine()
{

}

void DDM::DDMEngine::Run()
{
	//auto& window{ DDM::Window::GetInstance() };

	DDM::ServiceLocator::GetWindow().CreateWindow();

	// Variable that will indicate when the gameloop should stop running
	bool shouldQuit{ false };

	// As long as the app shouldn't quit, the gameloop will run
	while (!shouldQuit)
	{


		DDM::ServiceLocator::GetWindow().PollEvents();

		// Check if aplication should quit
		shouldQuit = DDM::ServiceLocator::GetWindow().ShouldClose();
	}
}
