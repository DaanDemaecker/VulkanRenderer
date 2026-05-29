// VulkanFileImage.cpp

// Header include
#include "VulkanFileImage.h"

// File includes
#include "Vulkan/Images/STBImage.h"

#include "Vulkan/Core/VulkanCore.h"
#include "Vulkan/Core/VulkanAllocator.h"

#include "Vulkan/Buffers/VulkanBuffer.h"

#include "Vulkan/Barriers/VulkanImageBarrier.h"
#include "Vulkan/Barriers/VulkanPipelineBarrier.h"

#include "Vulkan/CommandBuffers/VulkanCommandPool.h"

// Standard library includes
#include <memory>
#include <stdexcept>

DDM::Vulkan::VulkanFileImage::VulkanFileImage(const VulkanAllocator* pAllocator, const VulkanCore* pCore, const VulkanCommandPool* pCommandPool)
	:VulkanImage(pAllocator, pCore, pCommandPool)
{
}

DDM::Vulkan::VulkanFileImage::~VulkanFileImage()
{
}

void DDM::Vulkan::VulkanFileImage::LoadImage(const std::string& filePath)
{
	auto pSTBImage = std::make_unique<STBImage>(filePath);

	CreateImage(pSTBImage.get());
}

void DDM::Vulkan::VulkanFileImage::CreateImage(STBImage* pSTBImage)
{
	m_VkExtent = pSTBImage->GetExtent();

	VkImageCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.imageType = VK_IMAGE_TYPE_2D;
	createInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
	createInfo.extent = m_VkExtent;
	createInfo.mipLevels = 1;
	createInfo.arrayLayers = 1;
	createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	createInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.queueFamilyIndexCount = 0;
	createInfo.pQueueFamilyIndices = nullptr;
	createInfo.initialLayout = m_VkLayout;

	if (vkCreateImage(m_pCore->GetDeviceHandle(), &createInfo, m_pAllocator->GetAllocator(), &m_VkImage) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create Vulkan Image");
	}

	AllocateMemory();

	m_Initialized = true;

	// Create temp buffer
	auto tempBuffer = std::make_unique<VulkanBuffer>(m_pAllocator, m_pCore);

	tempBuffer->CreateBuffer(pSTBImage->GetSize());

	tempBuffer->WriteToBuffer(pSTBImage->GetHandle());

	CopyBufferToImage(tempBuffer.get());
}

void DDM::Vulkan::VulkanFileImage::CopyBufferToImage(VulkanBuffer* pBuffer)
{
	// Create a barrier to transition image from undefined layout to transfer destination layout
	std::unique_ptr<VulkanPipelineBarrier> pBarrier = std::make_unique<VulkanPipelineBarrier>(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0);

	std::unique_ptr<VulkanImageBarrier> pImageBarrier = std::make_unique<VulkanImageBarrier>(this, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	pBarrier->AddImageBarriers(std::move(pImageBarrier));


	auto commandBuffer = m_pCommandPool->GetCommandBuffer();

	commandBuffer->CmdPipelineBarrier(pBarrier.get());

	commandBuffer->Submit();

	m_VkLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

	commandBuffer->CmdCopyBufferToImage(this, pBuffer);

	commandBuffer->Submit();
}
