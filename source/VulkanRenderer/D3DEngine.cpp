#include "stdafx.h"
#include "D3DEngine.h"
#include "VulkanRenderer.h"
#include "TimeManager.h"
#include "Model.h"
#include "Material.h"
#include "TexturedMaterial.h"
#include <chrono>

D3D::Window g_pWindow{};

D3D::D3DEngine::D3DEngine(int width, int height)
{
	g_pWindow.Width = width;
	g_pWindow.Height = height;
	InitWindow();
}

D3D::D3DEngine::~D3DEngine()
{
	glfwDestroyWindow(g_pWindow.pWindow);
	glfwTerminate();
}

void D3D::D3DEngine::Run(const std::function<void()>& load)
{
	load();

	auto& renderer{ VulkanRenderer::GetInstance() };

	renderer.AddGraphicsPipeline("Diffuse", "../Resources/Shaders/Diffuse.Vert.spv", "../Resources/Shaders/Diffuse.Frag.spv", 1);
	renderer.AddGraphicsPipeline("NormalMap", "../Resources/Shaders/NormalMap.Vert.spv", "../Resources/Shaders/NormalMap.Frag.spv", 1);
	renderer.AddGraphicsPipeline("DiffNorm", "../Resources/Shaders/DiffNorm.Vert.spv", "../Resources/Shaders/DiffNorm.Frag.spv", 2);

	std::shared_ptr<D3D::TexturedMaterial> pVikingMaterial{ std::make_shared<D3D::TexturedMaterial>(std::initializer_list<const std::string>{"../resources/images/viking_room.png"}, "Diffuse") };
	std::shared_ptr<D3D::TexturedMaterial> pVehicleMaterial{ std::make_shared<D3D::TexturedMaterial>(std::initializer_list<const std::string>{"../resources/images/vehicle_diffuse.png"}, "Diffuse") };
	std::shared_ptr<D3D::Material> pVehicle2Material{ std::make_shared<D3D::TexturedMaterial>
		(std::initializer_list<const std::string>{"../resources/images/vehicle_diffuse.png", "../resources/images/vehicle_normal.png"}, "DiffNorm") };


	{
		std::vector<std::unique_ptr<Model>> pModels{};
		/*pModels.push_back(std::make_unique<Model>());
		pModels[0]->LoadModel("../Resources/Models/viking_room.obj");
		pModels[0]->SetMaterial(pVikingMaterial);
		pModels[0]->SetPosition(1.f, -0.2f, 5.f);
		pModels[0]->SetRotation(glm::radians(-90.0f), glm::radians(45.0f), 0.f);
		pModels[0]->SetScale(0.75f, 0.75f, 0.75f);*/

		pModels.push_back(std::make_unique<Model>());
		pModels[0]->LoadModel("../Resources/Models/vehicle.obj");
		pModels[0]->SetMaterial(pVehicle2Material);
		pModels[0]->SetPosition(1.f, 0, 5.f);
		pModels[0]->SetRotation(0.f, glm::radians(75.0f), 0.f);
		pModels[0]->SetScale(0.05f, 0.05f, 0.05f);

		pModels.push_back(std::make_unique<Model>());
		pModels[1]->LoadModel("../Resources/Models/vehicle.obj");
		pModels[1]->SetMaterial(pVehicleMaterial);
		pModels[1]->SetPosition(-1.f, 0, 5.f);
		pModels[1]->SetRotation(0.f, glm::radians(75.0f), 0.f);
		pModels[1]->SetScale(0.05f, 0.05f, 0.05f);
	}

	std::vector<std::unique_ptr<Model>> pModels{};

	for (int i{}; i < 20; ++i)
	{
		std::unique_ptr<Model> pModel{std::make_unique<Model>()};
		pModel->LoadModel("../Resources/Models/vehicle.obj");
		pModel->SetMaterial(pVehicleMaterial);

		pModels.push_back(std::move(pModel));
	}

	auto& time{ TimeManager::GetInstance() };


	auto lastTime = std::chrono::high_resolution_clock::now();

	//float lag = 0.0f;
	constexpr float fixedTimeStep = 0.02f;

	time.SetFixedTime(fixedTimeStep);

	bool capFrameRate{ true };
	constexpr float desiredFrameRate = 144;
	constexpr float desiredFrameDuration = 1000.f / desiredFrameRate;

	bool shouldQuit{false};

	while (!shouldQuit)
	{
		const auto currentTime = std::chrono::high_resolution_clock::now();
		const float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();

		lastTime = currentTime;

		time.SetDeltaTime(deltaTime);

		//std::cout << time.GetFps() << std::endl;

		glfwPollEvents();

		for (auto& pModel : pModels)
		{
			pModel->Update();
		}

		renderer.Render(pModels);

		shouldQuit = glfwWindowShouldClose(g_pWindow.pWindow);


		if (capFrameRate)
		{
			const auto frameDuration{ std::chrono::high_resolution_clock::now() - lastTime };
			const auto sleepTime{ std::chrono::milliseconds(static_cast<int>(desiredFrameDuration)) - frameDuration };
			std::this_thread::sleep_for(sleepTime);
		}
	}
}

void D3D::D3DEngine::InitWindow()
{
	//Initialize glfw
	glfwInit();

	//Tell GLFW not to create an OpenGL context
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	//Initialize the window
	g_pWindow.pWindow = glfwCreateWindow(g_pWindow.Width, g_pWindow.Height, "Vulkan", nullptr, nullptr);
	glfwSetWindowUserPointer(g_pWindow.pWindow, this);
	glfwSetFramebufferSizeCallback(g_pWindow.pWindow, FramebufferResizeCallback);
}

void D3D::D3DEngine::FramebufferResizeCallback(GLFWwindow* /*pWindow*/, int /*width*/, int /*height*/)
{
	g_pWindow.FrameBufferResized = true;
}
