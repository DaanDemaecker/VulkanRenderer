// VulkanImplementation.cpp

// Header include
#include "VulkanImplementation.h"

// File includes
#include "Vulkan/Core/VulkanAllocator.h"
#include "Vulkan/Core/VulkanCore.h"

DDM::VulkanImplementation::VulkanImplementation()
{
	m_pAllocator = std::make_unique<VulkanAllocator>();

	m_pCore = std::make_unique<VulkanCore>(m_pAllocator.get());
}

DDM::VulkanImplementation::~VulkanImplementation()
{

}
