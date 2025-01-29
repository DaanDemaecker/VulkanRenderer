// Vulkan3D.cpp

// Header include
#include "Vulkan3D.h"

// File includes
#include "Vulkan/Managers/DispatchableManager.h"
#include "Vulkan/Managers/ModelManager.h"
#include "Vulkan/Managers/CameraManager.h"

#include "Engine/ConfigManager.h"


uint32_t DDM3::Vulkan3D::m_sMaxFramesInFlight = 1;

uint32_t DDM3::Vulkan3D::m_sCurrentFrame = 0;

DDM3::Vulkan3D::Vulkan3D()
{
	m_sMaxFramesInFlight = ConfigManager::GetInstance().GetInt("MaxFramesInFlight");

	m_pDispatchableManager = std::make_unique<DDM3::DispatchableManager>();
}

DDM3::Vulkan3D::~Vulkan3D()
{

}

void DDM3::Vulkan3D::Init()
{
	m_pCameraManager = std::make_unique<DDM3::CameraManager>();

	m_pRenderer = std::make_unique<DDM3::VulkanRenderer3D>();

	m_pRenderer->SetupDefaultPipeline();
	m_pRenderer->SetupLight();
	m_pRenderer->SetupSkybox();

	m_pModelManager = std::make_unique<DDM3::ModelManager>();

}

void DDM3::Vulkan3D::Terminate()
{
	m_pRenderer = nullptr;
	m_pModelManager = nullptr;
}

VkInstance DDM3::Vulkan3D::GetVulkanInstance() const
{
	return m_pDispatchableManager->GetInstance();
}

VkDevice DDM3::Vulkan3D::GetDevice() const
{
	return m_pDispatchableManager->GetDevice();
}

VkPhysicalDevice DDM3::Vulkan3D::GetPhysicalDevice() const
{
	return m_pDispatchableManager->GetPhysicalDevice();
}

DDM3::GPUObject* DDM3::Vulkan3D::GetGPUObject() const
{
	return m_pDispatchableManager->GetGpuObject();
}

VkSurfaceKHR DDM3::Vulkan3D::GetSurface() const
{
	return m_pDispatchableManager->GetSurface();
}

DDM3::VulkanRenderer3D& DDM3::Vulkan3D::GetRenderer()
{
	return *m_pRenderer.get();
}

void DDM3::Vulkan3D::Render()
{
	m_pRenderer->Render(m_pModelManager->GetModels());

	// Go to the next frame
	++m_sCurrentFrame %= m_sMaxFramesInFlight;
}

DDM3::ModelManager* DDM3::Vulkan3D::GetModelManager()
{
	return m_pModelManager.get();
}

void DDM3::Vulkan3D::SetCamera(std::unique_ptr<Camera> pNewCamera)
{
	m_pCameraManager->SetCamera(std::move(pNewCamera));
}

DDM3::Camera* DDM3::Vulkan3D::GetCurrentCamera()
{
	return m_pCameraManager->GetCamera();
}

DDM3::CameraManager* DDM3::Vulkan3D::GetCameraManager()
{
	return m_pCameraManager.get();
}