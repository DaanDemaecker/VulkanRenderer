// VulkanImplementation.cpp

// Header include
#include "VulkanImplementation.h"

// File includes
#include "Vulkan/Core/VulkanAllocator.h"
#include "Vulkan/Core/VulkanCore.h"

#include "Vulkan/CommandBuffers/CommandPool.h"
#include "Vulkan/CommandBuffers/CommandBuffer.h"

#include "Vulkan/Images/STBImage.h"
#include "Vulkan/Images/VulkanImage.h"

DDM::VulkanImplementation::VulkanImplementation()
{
	m_pAllocator = std::make_unique<VulkanAllocator>();

	m_pCore = std::make_unique<VulkanCore>(m_pAllocator.get());

	m_pTransferCommandPool = std::make_unique<CommandPool>(m_pAllocator.get(), m_pCore.get(), m_pCore->GetTransferQueue(), true, false);

	auto testImage = std::make_unique<VulkanImage>(m_pAllocator.get(), m_pCore.get(), m_pTransferCommandPool.get());

	testImage->LoadImage("Resources/DefaultResources/DefaultTexture.png");
}

DDM::VulkanImplementation::~VulkanImplementation()
{

}
