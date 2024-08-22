// Vulkan3D.cpp

// File includes
#include "Vulkan3D.h"

#include "Vulkan/Managers/DispatchableManager.h"
#include "Vulkan/Managers/ModelManager.h"
#include "Vulkan/Managers/CameraManager.h"

#include "Engine/ConfigManager.h"


uint32_t D3D::Vulkan3D::m_sMaxFramesInFlight = 1;

uint32_t D3D::Vulkan3D::m_sCurrentFrame = 0;

D3D::Vulkan3D::Vulkan3D()
{
	m_sMaxFramesInFlight = ConfigManager::GetInstance().GetInt("MaxFramesInFlight");

	m_pDispatchableManager = std::make_unique<D3D::DispatchableManager>();
}

D3D::Vulkan3D::~Vulkan3D()
{

}

void D3D::Vulkan3D::Init()
{
	m_pCameraManager = std::make_unique<D3D::CameraManager>();

	m_pRenderer = std::make_unique<D3D::VulkanRenderer3D>();

	m_pRenderer->SetupDefaultPipeline();
	m_pRenderer->SetupLight();
	m_pRenderer->SetupSkybox();

	m_pModelManager = std::make_unique<D3D::ModelManager>();

}

void D3D::Vulkan3D::Terminate()
{
	m_pRenderer = nullptr;
	m_pModelManager = nullptr;
}

VkInstance D3D::Vulkan3D::GetVulkanInstance() const
{
	return m_pDispatchableManager->GetInstance();
}

VkDevice D3D::Vulkan3D::GetDevice() const
{
	return m_pDispatchableManager->GetDevice();
}

VkPhysicalDevice D3D::Vulkan3D::GetPhysicalDevice() const
{
	return m_pDispatchableManager->GetPhysicalDevice();
}

D3D::GPUObject* D3D::Vulkan3D::GetGPUObject() const
{
	return m_pDispatchableManager->GetGpuObject();
}

VkSurfaceKHR D3D::Vulkan3D::GetSurface() const
{
	return m_pDispatchableManager->GetSurface();
}

D3D::VulkanRenderer3D& D3D::Vulkan3D::GetRenderer()
{
	return *m_pRenderer.get();
}

void D3D::Vulkan3D::Render()
{
	m_pRenderer->Render(m_pModelManager->GetModels());

	// Go to the next frame
	++m_sCurrentFrame %= m_sMaxFramesInFlight;
}

D3D::ModelManager* D3D::Vulkan3D::GetModelManager()
{
	return m_pModelManager.get();
}

void D3D::Vulkan3D::SetCamera(std::unique_ptr<Camera> pNewCamera)
{
	m_pCameraManager->SetCamera(std::move(pNewCamera));
}

D3D::Camera* D3D::Vulkan3D::GetCurrentCamera()
{
	return m_pCameraManager->GetCamera();
}

D3D::CameraManager* D3D::Vulkan3D::GetCameraManager()
{
	return m_pCameraManager.get();
}