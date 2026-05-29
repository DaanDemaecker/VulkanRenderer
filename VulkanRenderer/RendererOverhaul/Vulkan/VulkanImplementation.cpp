// VulkanImplementation.cpp

// Header include
#include "VulkanImplementation.h"

// File includes
#include "Vulkan/Core/VulkanAllocator.h"
#include "Vulkan/Core/VulkanCore.h"

#include "Vulkan/CommandBuffers/VulkanCommandPool.h"
#include "Vulkan/CommandBuffers/VulkanCommandBuffer.h"

#include "Vulkan/Swapchain/VulkanSwapchain.h"

#include "Vulkan/Images/STBImage.h"
#include "Vulkan/Images/VulkanImage.h"
#include "Vulkan/Images/ImageTypes/VulkanFileImage.h"

DDM::Vulkan::VulkanImplementation::VulkanImplementation()
{
	m_pAllocator = std::make_unique<VulkanAllocator>();

	m_pCore = std::make_unique<VulkanCore>(m_pAllocator.get());

	m_pTransferCommandPool = std::make_unique<VulkanCommandPool>(m_pAllocator.get(), m_pCore.get(), m_pCore->GetTransferQueue(), true, true);

	m_pSwapchain = std::make_unique<VulkanSwapchain>(m_pAllocator.get(), m_pCore.get());

	m_pTestImage = std::make_unique<VulkanFileImage>(m_pAllocator.get(), m_pCore.get(), m_pTransferCommandPool.get());

	m_pTestImage->LoadImage("Resources/DefaultResources/DefaultTexture.png");
}

DDM::Vulkan::VulkanImplementation::~VulkanImplementation()
{

}
