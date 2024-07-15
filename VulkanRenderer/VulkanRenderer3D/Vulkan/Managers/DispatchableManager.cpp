// DispatchableManager.cpp

// File includes
#include "DispatchableManager.h"
#include "Vulkan/Wrappers/InstanceWrapper.h"

D3D::DispatchableManager::DispatchableManager()
{
	m_pInstanceWrapper = std::make_unique<InstanceWrapper>();
}

D3D::DispatchableManager::~DispatchableManager()
{
}

VkInstance D3D::DispatchableManager::GetInstance() const
{
	return m_pInstanceWrapper->GetInstance();
}

D3D::InstanceWrapper* D3D::DispatchableManager::GetInstanceWrapper() const
{
	return m_pInstanceWrapper.get();
}
