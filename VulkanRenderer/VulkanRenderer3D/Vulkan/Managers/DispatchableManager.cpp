// DispatchableManager.cpp

// File includes
#include "DispatchableManager.h"
#include "Vulkan/Wrappers/InstanceWrapper.h"
#include "Vulkan/Wrappers/SurfaceWrapper.h"

D3D::DispatchableManager::DispatchableManager()
{
	m_pInstanceWrapper = std::make_unique<InstanceWrapper>();

	m_pSurfaceWrapper = std::make_unique<SurfaceWrapper>(m_pInstanceWrapper->GetInstance());

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
