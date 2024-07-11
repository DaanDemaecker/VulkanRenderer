// VulkanRenderer2D.cpp

// File includes
#include "VulkanRenderer2D.h"
#include "Wrappers/InstanceWrapper.h"
#include "Wrappers/GPUObject.h"

D2D::VulkanRenderer2D::VulkanRenderer2D()
{
	InitVulkan();
}

D2D::VulkanRenderer2D::~VulkanRenderer2D()
{
	CleanupVulkan();
}

void D2D::VulkanRenderer2D::InitVulkan()
{
	m_pInstanceWrapper = std::make_unique<InstanceWrapper>();

	m_pGPUObject = std::make_unique<GPUObject>(m_pInstanceWrapper->GetInstance());
}

void D2D::VulkanRenderer2D::CleanupVulkan()
{

}
