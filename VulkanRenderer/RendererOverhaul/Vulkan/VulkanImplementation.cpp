// VulkanImplementation.cpp

// Header include
#include "VulkanImplementation.h"

// File includes
#include "Vulkan/Core/VulkanAllocator.h"
#include "Vulkan/Core/VulkanCore.h"

#include "Vulkan/CommandBuffers/VulkanCommandPool.h"
#include "Vulkan/CommandBuffers/VulkanCommandBuffer.h"

#include "Vulkan/Swapchain/VulkanSwapchain.h"

#include "Vulkan/Images/VulkanImage.h"
#include "Vulkan/Images/ImageTypes/VulkanFileImage.h"
#include "Vulkan/Barriers/VulkanImageBarrier.h"
#include "Vulkan/Barriers/VulkanPipelineBarrier.h"
#include "Vulkan/Shaders/VulkanShaderModule.h"

DDM::Vulkan::VulkanImplementation::VulkanImplementation()
{
	m_pAllocator = std::make_unique<VulkanAllocator>();

	m_pCore = std::make_unique<VulkanCore>(m_pAllocator.get());

	m_pTransferCommandPool = std::make_unique<VulkanCommandPool>(m_pAllocator.get(), m_pCore.get(), m_pCore->GetTransferQueue(), true, true);

	m_pSwapchain = std::make_unique<VulkanSwapchain>(m_pAllocator.get(), m_pCore.get());

	PresentTestFunction();

	ShaderTestFunction();
}

DDM::Vulkan::VulkanImplementation::~VulkanImplementation()
{

}

void DDM::Vulkan::VulkanImplementation::PresentTestFunction()
{
	m_pTestImage = std::make_unique<VulkanFileImage>(m_pAllocator.get(), m_pCore.get(), m_pTransferCommandPool.get());

	m_pTestImage->LoadImage("Resources/DefaultResources/DefaultTexture.png");

	// Create a barrier to transition image from undefined layout to transfer destination layout
	std::unique_ptr<VulkanPipelineBarrier> pBarrier = std::make_unique<VulkanPipelineBarrier>(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0);

	std::unique_ptr<VulkanImageBarrier> pImageBarrier = std::make_unique<VulkanImageBarrier>(m_pTestImage.get(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

	pBarrier->AddImageBarriers(std::move(pImageBarrier));

	pImageBarrier = std::make_unique<VulkanImageBarrier>(m_pSwapchain->GetCurrentImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	pBarrier->AddImageBarriers(std::move(pImageBarrier));

	auto commandBuffer = m_pTransferCommandPool->GetCommandBuffer();

	commandBuffer->CmdPipelineBarrier(pBarrier.get());


	commandBuffer->CmdCopyImageToImage(m_pTestImage.get(), m_pSwapchain->GetCurrentImage());


	pBarrier = std::make_unique<VulkanPipelineBarrier>(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0);

	pImageBarrier = std::make_unique<VulkanImageBarrier>(m_pSwapchain->GetCurrentImage(), VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	pBarrier->AddImageBarriers(std::move(pImageBarrier));

	commandBuffer->CmdPipelineBarrier(pBarrier.get());

	commandBuffer->Submit();

	m_pSwapchain->Present();
}

void DDM::Vulkan::VulkanImplementation::ShaderTestFunction()
{
	auto shaderModule = std::make_unique<VulkanShaderModule>(m_pCore.get(), m_pAllocator.get());

	shaderModule->CreateShaderModule("Resources/Shaders/Overhaul/Test.vert.spv");
}
