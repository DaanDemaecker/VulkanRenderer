// File includes
#include "Engine/DDMEngine.h"

#include "ServiceLocator/ServiceLocator.h"
#include "ServiceLocator/Window/GLFW/GLFWWindow.h"

// Standard library includes
#include <iostream>

int main()
{
	DDM::ServiceLocator::RegisterWindow(std::make_unique<DDM::GLFWWindow>());

	DDM::DDMEngine engine{};
	engine.Run();

	return EXIT_SUCCESS;
}