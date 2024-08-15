// D3DEngine.cpp

// Header include
#include "D3DEngine.h"

// File includes
#include "Vulkan/Renderers/VulkanRenderer3D.h"
#include "TimeManager.h"
#include "DataTypes/RenderClasses/Model.h"
#include "DataTypes/Materials/Material.h"
#include "DataTypes/Materials/TexturedMaterial.h"
#include "Window.h"
#include "ConfigManager.h"
#include "DataTypes/DescriptorObjects/TextureDescriptorObject.h"
#include "DataTypes/Camera.h"
#include "DataTypes/Materials/ShadowMaterial.h"
#include "Vulkan/Vulkan3D.h"
#include "Vulkan/Managers/ModelManager.h"

// Standard library includes
#include <chrono>
#include <thread>

D3D::D3DEngine::D3DEngine()
{
	// Create the window with the given width and height
	D3D::Window::GetInstance();
	
	D3D::Vulkan3D::GetInstance().Init();
}

D3D::D3DEngine::~D3DEngine()
{
	D3D::Vulkan3D::GetInstance().Terminate();	
}

// This function will run the gameloop for the duration of the app
void D3D::D3DEngine::Run(const std::function<void()>& load)
{
	// Run the load function
	load();

	auto& renderer{ Vulkan3D::GetInstance().GetRenderer() };
	auto& window{ Window::GetInstance() };

	auto pCamera =	renderer.GetCamera();
	pCamera->SetPosition(0, 5, -15);
	//auto rot{ glm::quat(glm::lookAt(pCamera->GetPosition(), glm::vec3{ 0, 0, 0 }, glm::vec3{ 0, 1, 0 }))};
	//pCamera->SetRotation(glm::eulerAngles(rot));
	

	// Get the timemanager locally to prevent calling it every frame
	auto& time{ TimeManager::GetInstance() };

	// Get current time for later use
	auto lastTime = std::chrono::high_resolution_clock::now();

	// Variable that will indicate if framerate should be capped or or not
	bool capFrameRate{ false };

	// Set desired framerate
	const int desiredFramerate = 60;

	// Calculate the duratinno of a single frame in milliseconds
	const std::chrono::milliseconds millisecondsPerFrame(1000 / desiredFramerate);

	// Variable that will indicate when the gameloop should stop running
	bool shouldQuit{false};

	// As long as the app shouldn't quit, the gameloop will run
	while (!shouldQuit)
	{
		// Get the current time
		const auto frameStart = std::chrono::high_resolution_clock::now();

		// Calculate the duration of last frame by subtracting the time start time from last frame from the starting time from this frame
		auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(frameStart - lastTime);

		// Calculate in seconds how long last frame lasted
		float deltaTime = static_cast<float>(duration.count());

		// Set lastTime to start of current frame for next iteration
		lastTime = frameStart;

		// Set current deltaTime in the timeManager
		time.SetDeltaTime(deltaTime);

		// Print FPS
		//std::cout << "FPS: " << time.GetFps() << std::endl;

		// Poll input for the window
		glfwPollEvents();

		renderer.GetCamera()->Update();
		
		Vulkan3D::GetInstance().GetModelManager()->Update();


		// Render all models
		Vulkan3D::GetInstance().Render();

		// Check if aplication should quit
		shouldQuit = glfwWindowShouldClose(window.GetWindowStruct().pWindow);

		// If cap framerate, sleep the appropriate amount of time
		if (capFrameRate)
		{
			// Calculate duration of the current frame
			auto frameTime = std::chrono::high_resolution_clock::now() - frameStart;

			// Calculate how long the thread should sleep
			auto sleepTime = millisecondsPerFrame - std::chrono::duration_cast<std::chrono::milliseconds>(frameTime);

			// Make the thread sleep for the right amount of time
			std::this_thread::sleep_for(sleepTime);
		}
	}
}
