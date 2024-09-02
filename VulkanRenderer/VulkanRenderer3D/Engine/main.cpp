// File includes
#include "D3DEngine.h"

#include "Vulkan/Vulkan3D.h"

#include "Engine/Window.h"

#include "DataTypes/Materials/TexturedMaterial.h"
#include "DataTypes/Materials/ShadowMaterial.h"
#include "DataTypes/RenderClasses/Model.h"

#include "Vulkan/Managers/ModelManager.h"
#include "Vulkan/Managers/CameraManager.h"


void SetupPipelines()
{
	auto& renderer{ D3D::Vulkan3D::GetInstance().GetRenderer() };

	renderer.AddGraphicsPipeline("Diffuse", { "Resources/Shaders/Diffuse.Vert.spv", "Resources/Shaders/Diffuse.Frag.spv" });
	renderer.AddGraphicsPipeline("NormalMap", { "Resources/Shaders/NormalMap.Vert.spv", "Resources/Shaders/NormalMap.Frag.spv" });
	renderer.AddGraphicsPipeline("DiffNorm", { "Resources/Shaders/DiffNorm.Vert.spv", "Resources/Shaders/DiffNorm.Frag.spv" });

	renderer.AddGraphicsPipeline("Test", { "Resources/Shaders/Test.Vert.spv", "Resources/Shaders/Test.Frag.spv" });

	renderer.AddGraphicsPipeline("DiffuseUnshaded", { "Resources/Shaders/DiffuseUnshaded.Vert.spv", "Resources/Shaders/DiffuseUnshaded.Frag.spv" });

	renderer.AddGraphicsPipeline("Specular", { "Resources/Shaders/Specular.Vert.spv", "Resources/Shaders/Specular.Frag.spv" });

	renderer.AddGraphicsPipeline("DiffNormSpec", { "Resources/Shaders/DiffNormSpec.Vert.spv", "Resources/Shaders/DiffNormSpec.Frag.spv" });


	renderer.AddGraphicsPipeline("DiffuseShadow", { "Resources/Shaders/DiffuseShadow.Vert.spv", "Resources/Shaders/DiffuseShadow.Frag.spv" });
}

void load()
{
	SetupPipelines();

	auto& renderer{ D3D::Vulkan3D::GetInstance().GetRenderer() };

	std::shared_ptr<D3D::TexturedMaterial> pGroundPlaneMaterial{ std::make_shared<D3D::TexturedMaterial>(std::initializer_list<const std::string>{"resources/images/GroundPlane.png"}, "Diffuse") };
	std::shared_ptr<D3D::ShadowMaterial> pGroundPlaneMaterial2{ std::make_shared<D3D::ShadowMaterial>(std::initializer_list<const std::string>{"resources/images/GroundPlane.png"}, "DiffuseShadow") };

	std::shared_ptr<D3D::TexturedMaterial> pVikingMaterial{ std::make_shared<D3D::TexturedMaterial>(std::initializer_list<const std::string>{"resources/images/viking_room.png"}, "Diffuse") };
	std::shared_ptr<D3D::TexturedMaterial> pVehicleMaterial{ std::make_shared<D3D::TexturedMaterial>(std::initializer_list<const std::string>{"resources/images/vehicle_diffuse.png"}, "Diffuse") };
	std::shared_ptr<D3D::TexturedMaterial> pFireMaterial{ std::make_shared<D3D::TexturedMaterial>(std::initializer_list<const std::string>{"resources/images/fireFX_diffuse.png"}, "DiffuseUnshaded") };

	std::shared_ptr<D3D::Material> pVehicle2Material{ std::make_shared<D3D::TexturedMaterial>
		(std::initializer_list<const std::string>{"resources/images/vehicle_diffuse.png", "resources/images/vehicle_normal.png"}, "DiffNorm") };

	std::shared_ptr<D3D::TexturedMaterial> pTestMaterial{ std::make_shared<D3D::TexturedMaterial>(std::initializer_list<const std::string>{"resources/images/TestRed.png","resources/images/TestBlue.png"}, "Test") };

	std::shared_ptr<D3D::TexturedMaterial> pVehicle3Material{ std::make_shared<D3D::TexturedMaterial>
		(std::initializer_list<const std::string>{"resources/images/vehicle_diffuse.png", "resources/images/vehicle_normal.png",
		"resources/images/vehicle_gloss.png", "resources/images/vehicle_specular.png"},
			"DiffNormSpec") };

	std::shared_ptr<D3D::ShadowMaterial> pVehicleMaterial4{ std::make_shared<D3D::ShadowMaterial>(std::initializer_list<const std::string>{"resources/images/vehicle_diffuse.png"}, "DiffuseShadow") };


	auto pModelManager{ D3D::Vulkan3D::GetInstance().GetModelManager() };


	std::unique_ptr<D3D::Model> pCurrModel{};

	// Load groundplane
	pCurrModel = std::make_unique<D3D::Model>();

	pCurrModel->LoadModel("Resources/Models/Plane.obj");
	pCurrModel->SetMaterial(pGroundPlaneMaterial2);
	pCurrModel->SetRotate(false);
	pModelManager->AddModel(std::move(pCurrModel));



	// Load vehicle object
	pCurrModel = std::make_unique<D3D::Model>();

	pCurrModel->LoadModel("Resources/Models/vehicle.obj");
	pCurrModel->SetMaterial(pVehicleMaterial4);
	//pModel->SetMaterial(pTestMaterial);
	pCurrModel->SetPosition(0.f, 5, 0.f);
	pCurrModel->SetRotation(0.f, glm::radians(75.0f), 0.f);
	pCurrModel->SetScale(0.25f, 0.25f, 0.25f);

	pModelManager->AddModel(std::move(pCurrModel));


	// Load fire vfx object
	pCurrModel = std::make_unique<D3D::Model>();

	pCurrModel->LoadModel("Resources/Models/fireFX.obj");
	pCurrModel->SetCastsShadow(false);
	pCurrModel->SetMaterial(pFireMaterial);
	pCurrModel->SetPosition(0.f, 5, 0.f);
	pCurrModel->SetRotation(0.f, glm::radians(75.0f), 0.f);
	pCurrModel->SetScale(0.25f, 0.25f, 0.25f);

	pModelManager->AddModel(std::move(pCurrModel));

	D3D::Vulkan3D::GetInstance().GetCameraManager()->SetSkybox(std::make_unique<D3D::SkyBox>(
		std::initializer_list<const std::string>{"resources/images/CubeMap/Sky_Right.png",
		"resources/images/CubeMap/Sky_Left.png",
		"resources/images/CubeMap/Sky_Up.png",
		"resources/images/CubeMap/Sky_Down.png",
		"resources/images/CubeMap/Sky_Front.png",
		"resources/images/CubeMap/Sky_Back.png"}));
}

int main()
{
	D3D::D3DEngine engine{};
	engine.Run(load);

	return EXIT_SUCCESS;
} 
	