// VulkanImage.cpp

// Header include
#include "VulkanImage.h"

// File includes
#include "Vulkan/Core/VulkanCore.h"
#include "Vulkan/Core/VulkanAllocator.h"
#include "Vulkan/CommandBuffers/CommandPool.h"
#include "Vulkan/Images/STBImage.h"
#include "Vulkan/Buffers/VulkanBuffer.h"
#include "Vulkan/Barriers/VulkanPipelineBarrier.h"

// Standard library includes
#include <stdexcept>

DDM::VulkanImage::VulkanImage(const VulkanAllocator* pAllocator, const VulkanCore* pCore, const CommandPool* pCommandPool)
	:m_pAllocator{ pAllocator },
	m_pCore{ pCore },
	m_pCommandPool{ pCommandPool }
{

}

DDM::VulkanImage::~VulkanImage()
{
	if (m_Initialized)
	{
		vkDestroyImage(m_pCore->GetDeviceHandle(), m_VkImage, m_pAllocator->GetAllocator());
	}
}

void DDM::VulkanImage::LoadImage(const std::string& filePath)
{
	auto pSTBImage = std::make_unique<STBImage>(filePath);

	CreateImage(pSTBImage.get());
}

void DDM::VulkanImage::CreateImage(STBImage* pSTBImage)
{
	VkImageCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.imageType = VK_IMAGE_TYPE_2D;
	createInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
	createInfo.extent = pSTBImage->GetExtent();
	createInfo.mipLevels = 1;
	createInfo.arrayLayers = 1;
	createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	createInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.queueFamilyIndexCount = 0;
	createInfo.pQueueFamilyIndices = nullptr;
	createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	if (vkCreateImage(m_pCore->GetDeviceHandle(), &createInfo, m_pAllocator->GetAllocator(), &m_VkImage) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create Vulkan Image");
	}

	m_Initialized = true;

	// Create temp buffer
	auto tempBuffer = std::make_unique<VulkanBuffer>(m_pAllocator, m_pCore);

	tempBuffer->CreateBuffer(pSTBImage->GetSize());

	tempBuffer->WriteToBuffer(pSTBImage->GetHandle());

	CopyBufferToImage(tempBuffer.get());
}

void DDM::VulkanImage::CopyBufferToImage(VulkanBuffer* pBuffer)
{
	std::unique_ptr<VulkanPipelineBarrier> pBarrier = std::make_unique<VulkanPipelineBarrier>(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0);

	auto commandBuffer = m_pCommandPool->GetCommandBuffer();

	commandBuffer->CmdPipelineBarrier(pBarrier.get());

	commandBuffer->Submit();

	commandBuffer->CmdPipelineBarrier(pBarrier.get());

	commandBuffer->Submit();
}
