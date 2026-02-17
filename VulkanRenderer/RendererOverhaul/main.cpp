// File includes
#include "Engine/DDMEngine.h"

#include "ServiceLocator/ServiceLocator.h"
#include "ServiceLocator/Window/GLFW/GLFWWindow.h"
#include "ServiceLocator/FileSystem/DDM/DDMFileSystem.h"

// Standard library includes
#include <iostream>

int main()
{
	DDM::ServiceLocator::RegisterWindow(std::make_unique<DDM::GLFWWindow>());
	DDM::ServiceLocator::RegisterFileSystem(std::make_unique<DDM::DDMFileSystem>());

	DDM::DDMEngine engine{};
	engine.Run();

	return EXIT_SUCCESS;
}