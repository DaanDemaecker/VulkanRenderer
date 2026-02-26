// File includes
#include "Engine/DDMEngine.h"

#include "ServiceLocator/ServiceLocator.h"
#include "ServiceLocator/FileSystem/DDM/DDMFileSystem.h"
#include "ServiceLocator/Renderer/VulkanRenderer/VulkanRenderer.h"

// Standard library includes
#include <iostream>

int main()
{
	// Register services
	DDM::ServiceLocator::RegisterFileSystem(std::make_unique<DDM::DDMFileSystem>());
	DDM::ServiceLocator::RegisterRenderer(std::make_unique<DDM::VulkanRenderer>());

	DDM::DDMEngine engine{};
	engine.Run();

	return EXIT_SUCCESS;
}