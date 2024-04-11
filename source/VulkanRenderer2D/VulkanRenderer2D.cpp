// VulkanRenderer2D.cpp

// File includes
#include "VulkanRenderer2D.h"
#include "InstanceWrapper.h"

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
}

void D2D::VulkanRenderer2D::CleanupVulkan()
{

}
