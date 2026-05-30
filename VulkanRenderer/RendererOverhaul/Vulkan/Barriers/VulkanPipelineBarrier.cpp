// VulkanPIpelineBarrier.cpp

// Header include
#include "VulkanPipelineBarrier.h"

// File includes
#include "Vulkan/Barriers/VulkanImageBarrier.h"

DDM::Vulkan::VulkanPipelineBarrier::VulkanPipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags flags)
	:m_SrcStageMask{srcStageMask},
	m_DstStageMask{dstStageMask},
	m_DependencyFlags{flags}
{

}

DDM::Vulkan::VulkanPipelineBarrier::~VulkanPipelineBarrier()
{

}

void DDM::Vulkan::VulkanPipelineBarrier::Execute(VkCommandBuffer commandBuffer)
{
	std::vector<VkImageMemoryBarrier> imageBarriers(m_ImageBarriers.size());

	for (int i{}; i < imageBarriers.size(); ++i)
	{
		imageBarriers[i] = m_ImageBarriers[i]->GetHandle();
	}


	vkCmdPipelineBarrier(commandBuffer, m_SrcStageMask, m_DstStageMask, m_DependencyFlags,
		// Global memory barriers
		0, nullptr,
		// Buffer barriers
		0, nullptr,
		// Image barriers
		static_cast<uint32_t>(imageBarriers.size()), imageBarriers.size() > 0 ? imageBarriers.data() : nullptr);

	Executed();
}

void DDM::Vulkan::VulkanPipelineBarrier::AddImageBarriers(std::unique_ptr<VulkanImageBarrier> pImageBarrier)
{
	m_ImageBarriers.push_back(std::move(pImageBarrier));
}

void DDM::Vulkan::VulkanPipelineBarrier::Executed()
{
	for (auto& pImageBarrier : m_ImageBarriers)
	{
		pImageBarrier->Executed();
	}
}
