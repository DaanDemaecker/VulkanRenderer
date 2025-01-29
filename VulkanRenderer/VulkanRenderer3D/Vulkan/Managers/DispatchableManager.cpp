// DispatchableManager.cpp

#include "DispatchableManager.h"

// File includes
#include "Vulkan/Wrappers/InstanceWrapper.h"
#include "Vulkan/Wrappers/SurfaceWrapper.h"
#include "Vulkan/Wrappers/GPUObject.h"

DDM3::DispatchableManager::DispatchableManager()
{
	m_pInstanceWrapper = std::make_unique<InstanceWrapper>();

	m_pSurfaceWrapper = std::make_unique<SurfaceWrapper>(m_pInstanceWrapper->GetInstance());

	m_pGPUObject = std::make_unique<GPUObject>(m_pInstanceWrapper.get(), m_pSurfaceWrapper->GetSurface());
}

DDM3::DispatchableManager::~DispatchableManager()
{
	m_pSurfaceWrapper->Cleanup(m_pInstanceWrapper->GetInstance());
}

VkInstance DDM3::DispatchableManager::GetInstance() const
{
	return m_pInstanceWrapper->GetInstance();
}

DDM3::InstanceWrapper* DDM3::DispatchableManager::GetInstanceWrapper() const
{
	return m_pInstanceWrapper.get();
}

VkSurfaceKHR DDM3::DispatchableManager::GetSurface() const
{
	return m_pSurfaceWrapper->GetSurface();
}

VkDevice DDM3::DispatchableManager::GetDevice() const
{
	return m_pGPUObject->GetDevice();
}

VkPhysicalDevice DDM3::DispatchableManager::GetPhysicalDevice() const
{
	return m_pGPUObject->GetPhysicalDevice();
}

DDM3::GPUObject* DDM3::DispatchableManager::GetGpuObject() const
{
	return m_pGPUObject.get();
}

void DDM3::DispatchableManager::WaitIdle()
{
	m_pGPUObject->WaitIdle();
}
