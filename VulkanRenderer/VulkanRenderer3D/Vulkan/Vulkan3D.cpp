// Vulkan3D.cpp

// File includes
#include "Vulkan3D.h"
#include "Vulkan/Managers/DispatchableManager.h"

D3D::Vulkan3D::Vulkan3D()
{
	m_pDispatchableManager = std::make_unique<D3D::DispatchableManager>();
}

D3D::Vulkan3D::~Vulkan3D()
{

}

void D3D::Vulkan3D::Init()
{

}

void D3D::Vulkan3D::Terminate()
{

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