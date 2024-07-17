// DispatchableManager.cpp

// File includes
#include "DispatchableManager.h"
#include "Vulkan/Wrappers/InstanceWrapper.h"
#include "Vulkan/Wrappers/SurfaceWrapper.h"
#include "Vulkan/Wrappers/GPUObject.h"

D3D::DispatchableManager::DispatchableManager()
{
	m_pInstanceWrapper = std::make_unique<InstanceWrapper>();

	m_pSurfaceWrapper = std::make_unique<SurfaceWrapper>(m_pInstanceWrapper->GetInstance());

	m_pGPUObject = std::make_unique<GPUObject>(m_pInstanceWrapper.get(), m_pSurfaceWrapper->GetSurface());
}

D3D::DispatchableManager::~DispatchableManager()
{
	m_pSurfaceWrapper->Cleanup(m_pInstanceWrapper->GetInstance());
}

VkInstance D3D::DispatchableManager::GetInstance() const
{
	return m_pInstanceWrapper->GetInstance();
}

D3D::InstanceWrapper* D3D::DispatchableManager::GetInstanceWrapper() const
{
	return m_pInstanceWrapper.get();
}

VkSurfaceKHR D3D::DispatchableManager::GetSurface() const
{
	return m_pSurfaceWrapper->GetSurface();
}

VkDevice D3D::DispatchableManager::GetDevice() const
{
	return m_pGPUObject->GetDevice();
}

VkPhysicalDevice D3D::DispatchableManager::GetPhysicalDevice() const
{
	return m_pGPUObject->GetPhysicalDevice();
}

D3D::GPUObject* D3D::DispatchableManager::GetGpuObject() const
{
	return m_pGPUObject.get();
}

void D3D::DispatchableManager::WaitIdle()
{
	m_pGPUObject->WaitIdle();
}
