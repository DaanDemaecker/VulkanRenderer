// VulkanImplementation.cpp

// Header include
#include "VulkanImplementation.h"

// File includes
#include "Vulkan/Core/VulkanCore.h"

DDM::VulkanImplementation::VulkanImplementation()
{
	m_pCore = std::make_unique<VulkanCore>();
}

DDM::VulkanImplementation::~VulkanImplementation()
{
}
