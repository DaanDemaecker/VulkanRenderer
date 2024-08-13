// D3DEngine.cpp

// File includes
#include "D3DEngine.h"
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

	auto& renderer{ Vulkan3D::GetInstance().GetRenderer()};
	auto& window{ Window::GetInstance() };

	renderer.AddGraphicsPipeline("Diffuse", { "Resources/Shaders/Diffuse.Vert.spv", "Resources/Shaders/Diffuse.Frag.spv" });
	renderer.AddGraphicsPipeline("NormalMap", { "Resources/Shaders/NormalMap.Vert.spv", "Resources/Shaders/NormalMap.Frag.spv" });
	renderer.AddGraphicsPipeline("DiffNorm", { "Resources/Shaders/DiffNorm.Vert.spv", "Resources/Shaders/DiffNorm.Frag.spv" });
	
	renderer.AddGraphicsPipeline("Test", { "Resources/Shaders/Test.Vert.spv", "Resources/Shaders/Test.Frag.spv" });
	
	renderer.AddGraphicsPipeline("DiffuseUnshaded", { "Resources/Shaders/DiffuseUnshaded.Vert.spv", "Resources/Shaders/DiffuseUnshaded.Frag.spv" });

	renderer.AddGraphicsPipeline("Specular", { "Resources/Shaders/Specular.Vert.spv", "Resources/Shaders/Specular.Frag.spv" });

	renderer.AddGraphicsPipeline("DiffNormSpec", { "Resources/Shaders/DiffNormSpec.Vert.spv", "Resources/Shaders/DiffNormSpec.Frag.spv" });


	renderer.AddGraphicsPipeline("DiffuseShadow", { "Resources/Shaders/DiffuseShadow.Vert.spv", "Resources/Shaders/DiffuseShadow.Frag.spv" });

	std::shared_ptr<D3D::TexturedMaterial> pGroundPlaneMaterial{ std::make_shared<D3D::TexturedMaterial>(std::initializer_list<const std::string>{"resources/images/GroundPlane.png"}, "Diffuse") };
	std::shared_ptr<D3D::ShadowMaterial> pGroundPlaneMaterial2{ std::make_shared<D3D::ShadowMaterial>(std::initializer_list<const std::string>{"resources/images/GroundPlane.png"}, "DiffuseShadow") };

	std::shared_ptr<D3D::TexturedMaterial> pVikingMaterial{ std::make_shared<D3D::TexturedMaterial>(std::initializer_list<const std::string>{"resources/images/viking_room.png"}, "Diffuse") };
	std::shared_ptr<D3D::TexturedMaterial> pVehicleMaterial{ std::make_shared<D3D::TexturedMaterial>(std::initializer_list<const std::string>{"resources/images/vehicle_diffuse.png"}, "Diffuse") };
	std::shared_ptr<D3D::TexturedMaterial> pFireMaterial{ std::make_shared<D3D::TexturedMaterial>(std::initializer_list<const std::string>{"resources/images/fireFX_diffuse.png"}, "DiffuseUnshaded") };

	std::shared_ptr<D3D::Material> pVehicle2Material{ std::make_shared<D3D::TexturedMaterial>
		(std::initializer_list<const std::string>{"resources/images/vehicle_diffuse.png", "resources/images/vehicle_normal.png"}, "DiffNorm") };

	std::shared_ptr<D3D::TexturedMaterial> pTestMaterial{std::make_shared<D3D::TexturedMaterial>(std::initializer_list<const std::string>{"resources/images/TestRed.png","resources/images/TestBlue.png"}, "Test")};
	
	std::shared_ptr<D3D::TexturedMaterial> pVehicle3Material{ std::make_shared<D3D::TexturedMaterial>
		(std::initializer_list<const std::string>{"resources/images/vehicle_diffuse.png", "resources/images/vehicle_normal.png",
		"resources/images/vehicle_gloss.png", "resources/images/vehicle_specular.png"},
			"DiffNormSpec") };

	std::shared_ptr<D3D::ShadowMaterial> pVehicleMaterial4{ std::make_shared<D3D::ShadowMaterial>(std::initializer_list<const std::string>{"resources/images/vehicle_diffuse.png"}, "DiffuseShadow") };



		std::vector<std::unique_ptr<Model>> pModels{};

		/*pModels.push_back(std::make_unique<Model>());
		pModels[0]->LoadModel("../Resources/Models/viking_room.obj");
		pModels[0]->SetMaterial(pVikingMaterial);
		pModels[0]->SetPosition(1.f, -0.2f, 5.f);
		pModels[0]->SetRotation(glm::radians(-90.0f), glm::radians(45.0f), 0.f);
		pModels[0]->SetScale(0.75f, 0.75f, 0.75f);*/

		std::unique_ptr<Model> pCurrModel{};

		pCurrModel = std::make_unique<Model>();

		pCurrModel->LoadModel("Resources/Models/Plane.obj");
		pCurrModel->SetMaterial(pGroundPlaneMaterial2);

		pCurrModel->SetRotate(false);

		pModels.push_back(std::move(pCurrModel));

		/*pCurrModel = std::make_unique<Model>();

		pCurrModel->LoadModel("../Resources/Models/vehicle.obj");
		pCurrModel->SetMaterial(pVehicle2Material);
		pCurrModel->SetPosition(1.f, 0, 5.f);
		pCurrModel->SetRotation(0.f, glm::radians(75.0f), 0.f);
		pCurrModel->SetScale(0.05f, 0.05f, 0.05f);

		pModels.push_back(std::move(pCurrModel));*/


		pCurrModel = std::make_unique<Model>();
		
		pCurrModel->LoadModel("Resources/Models/vehicle.obj");
		pCurrModel->SetMaterial(pVehicleMaterial4);
		//pModel->SetMaterial(pTestMaterial);
		pCurrModel->SetPosition(0.f, 5, 0.f);
		pCurrModel->SetRotation(0.f, glm::radians(75.0f), 0.f);
		pCurrModel->SetScale(0.25f, 0.25f, 0.25f);
		
		pModels.push_back(std::move(pCurrModel));


		pCurrModel = std::make_unique<Model>();

		pCurrModel->LoadModel("Resources/Models/fireFX.obj");
		pCurrModel->SetCastsShadow(false);
		pCurrModel->SetMaterial(pFireMaterial);
		pCurrModel->SetPosition(0.f, 5, 0.f);
		pCurrModel->SetRotation(0.f, glm::radians(75.0f), 0.f);
		pCurrModel->SetScale(0.25f, 0.25f, 0.25f);

		pModels.push_back(std::move(pCurrModel));


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

		// Update all models
		for (auto& pModel : pModels)
		{
			pModel->Update();
		}

		renderer.GetCamera()->Update();
		

		// Render all models
		Vulkan3D::GetInstance().Render(pModels);

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
}
