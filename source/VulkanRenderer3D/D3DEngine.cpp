// D3DEngine.cpp

// File includes
#include "D3DEngine.h"
#include "VulkanRenderer3D.h"
#include "TimeManager.h"
#include "Model.h"
#include "Material.h"
#include "TexturedMaterial.h"
#include "Window.h"
#include "ConfigManager.h"

// Standard library includes
#include <chrono>
#include <thread>

D3D::D3DEngine::D3DEngine()
{
	// Create the window with the given width and height
	D3D::Window::GetInstance();
}

D3D::D3DEngine::~D3DEngine()
{
	
}

// This function will run the gameloop for the duration of the app
void D3D::D3DEngine::Run(const std::function<void()>& load)
{
	// Run the load function
	load();

	auto& renderer{ VulkanRenderer3D::GetInstance() };
	auto& window{ Window::GetInstance() };

	renderer.SetupSkybox();

	renderer.AddGraphicsPipeline("Diffuse", "../Resources/Shaders/Diffuse.Vert.spv", "../Resources/Shaders/Diffuse.Frag.spv",1, 1, 1);
	renderer.AddGraphicsPipeline("NormalMap", "../Resources/Shaders/NormalMap.Vert.spv", "../Resources/Shaders/NormalMap.Frag.spv", 1, 1, 1);
	renderer.AddGraphicsPipeline("DiffNorm", "../Resources/Shaders/DiffNorm.Vert.spv", "../Resources/Shaders/DiffNorm.Frag.spv", 1, 1, 2);
	
	renderer.AddGraphicsPipeline("Test", "../Resources/Shaders/Test.Vert.spv", "../Resources/Shaders/Test.Frag.spv", 1, 1, 2);
	
	renderer.AddGraphicsPipeline("DiffuseUnshaded", "../Resources/Shaders/DiffuseUnshaded.Vert.spv", "../Resources/Shaders/DiffuseUnshaded.Frag.spv", 1, 1, 1);

	renderer.AddGraphicsPipeline("Specular", "../Resources/Shaders/Specular.Vert.spv", "../Resources/Shaders/Specular.Frag.spv", 1, 1, 2);

	renderer.AddGraphicsPipeline("DiffNormSpec", "../Resources/Shaders/DiffNormSpec.Vert.spv", "../Resources/Shaders/DiffNormSpec.Frag.spv", 1, 1, 4);


	
	std::shared_ptr<D3D::TexturedMaterial> pVikingMaterial{ std::make_shared<D3D::TexturedMaterial>(std::initializer_list<const std::string>{"../resources/images/viking_room.png"}, "Diffuse") };
	std::shared_ptr<D3D::TexturedMaterial> pVehicleMaterial{ std::make_shared<D3D::TexturedMaterial>(std::initializer_list<const std::string>{"../resources/images/vehicle_diffuse.png"}, "Diffuse") };
	std::shared_ptr<D3D::TexturedMaterial> pFireMaterial{ std::make_shared<D3D::TexturedMaterial>(std::initializer_list<const std::string>{"../resources/images/fireFX_diffuse.png"}, "DiffuseUnshaded") };

	std::shared_ptr<D3D::Material> pVehicle2Material{ std::make_shared<D3D::TexturedMaterial>
		(std::initializer_list<const std::string>{"../resources/images/vehicle_diffuse.png", "../resources/images/vehicle_normal.png"}, "DiffNorm") };

	std::shared_ptr<D3D::TexturedMaterial> pTestMaterial{std::make_shared<D3D::TexturedMaterial>(std::initializer_list<const std::string>{"../resources/images/TestRed.png","../resources/images/TestBlue.png"}, "Test")};
	
	std::shared_ptr<D3D::TexturedMaterial> pVehicle3Material{ std::make_shared<D3D::TexturedMaterial>
		(std::initializer_list<const std::string>{"../resources/images/vehicle_diffuse.png", "../resources/images/vehicle_normal.png",
		"../resources/images/vehicle_gloss.png", "../resources/images/vehicle_specular.png"},
			"DiffNormSpec") };


		std::vector<std::unique_ptr<Model>> pModels{};

		/*pModels.push_back(std::make_unique<Model>());
		pModels[0]->LoadModel("../Resources/Models/viking_room.obj");
		pModels[0]->SetMaterial(pVikingMaterial);
		pModels[0]->SetPosition(1.f, -0.2f, 5.f);
		pModels[0]->SetRotation(glm::radians(-90.0f), glm::radians(45.0f), 0.f);
		pModels[0]->SetScale(0.75f, 0.75f, 0.75f);*/

		std::unique_ptr<Model> pCurrModel{};

		/*pCurrModel = std::make_unique<Model>();

		pCurrModel->LoadModel("../Resources/Models/vehicle.obj");
		pCurrModel->SetMaterial(pVehicle2Material);
		pCurrModel->SetPosition(1.f, 0, 5.f);
		pCurrModel->SetRotation(0.f, glm::radians(75.0f), 0.f);
		pCurrModel->SetScale(0.05f, 0.05f, 0.05f);

		pModels.push_back(std::move(pCurrModel));*/


		pCurrModel = std::make_unique<Model>();

		pCurrModel->LoadModel("../Resources/Models/vehicle.obj");
		pCurrModel->SetMaterial(pVehicle3Material);
		//pModel->SetMaterial(pTestMaterial);
		pCurrModel->SetPosition(0.f, 0, 10.f);
		pCurrModel->SetRotation(0.f, glm::radians(75.0f), 0.f);
		pCurrModel->SetScale(0.25f, 0.25f, 0.25f);

		pModels.push_back(std::move(pCurrModel));


		pCurrModel = std::make_unique<Model>();

		pCurrModel->LoadModel("../Resources/Models/fireFX.obj");
		pCurrModel->SetMaterial(pFireMaterial);
		pCurrModel->SetPosition(0.f, 0, 10.f);
		pCurrModel->SetRotation(0.f, glm::radians(75.0f), 0.f);
		pCurrModel->SetScale(0.25f, 0.25f, 0.25f);

		pModels.push_back(std::move(pCurrModel));


	/*for (int i{}; i < 20; ++i)
	{
		std::unique_ptr<Model> pModel{std::make_unique<Model>()};
		pModel->LoadModel("../Resources/Models/vehicle.obj");
		pModel->SetMaterial(pVehicleMaterial);

		pModels.push_back(std::move(pModel));
	}*/

	/*{
		std::unique_ptr<Model> pModel{ std::make_unique<D3D::Model>() };
		pModel->LoadModel("../Resources/Models/Cube.obj");
		pModel->SetMaterial(pVikingMaterial);
		pModel->SetPosition(0, 0, 5.f);
		pModel->SetScale(1000, 1000, 1000);
		pModels.push_back(std::move(pModel));
	}*/
	

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

		// Update all models
		for (auto& pModel : pModels)
		{
			pModel->Update();
		}

		auto rot{ renderer.GetCamera()->GetRotation() };
		// Initialize rotation speed
		constexpr float rotSpeed{ -glm::radians(15.f) };

		// Calculate amount of rotation
		float rotAmount{ rotSpeed * TimeManager::GetInstance().GetDeltaTime() };

		// Set new rotation
		renderer.GetCamera()->SetRotation(rot.x, rot.y + rotAmount, rot.z);
		

		// Render all models
		renderer.Render(pModels);

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


			// Calculate the moment the thread should wake up and continue
			//const auto frameEnd{frameStart + desiredFrameDuration };
			
			// Make the thread sleep until the desired time
			//std::this_thread::sleep_until(frameEnd);
		}
	}

	renderer.CleanupSkybox();
}
