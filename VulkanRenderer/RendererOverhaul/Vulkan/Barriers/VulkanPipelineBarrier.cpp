#include "VulkanPipelineBarrier.h"

DDM::VulkanPipelineBarrier::VulkanPipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags flags)
	:m_SrcStageMask{srcStageMask},
	m_DstStageMask{dstStageMask},
	m_DependencyFlags{flags}
{

}

DDM::VulkanPipelineBarrier::~VulkanPipelineBarrier()
{

}

void DDM::VulkanPipelineBarrier::Execute(VkCommandBuffer commandBuffer)
{
	vkCmdPipelineBarrier(commandBuffer, m_SrcStageMask, m_DstStageMask, m_DependencyFlags,
		0, nullptr,
		0, nullptr,
		0, nullptr);
}
