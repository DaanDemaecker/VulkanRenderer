#include "Viewport.h"

D3D::Viewport::Viewport()
{
	// Set viewport x to 0
	m_Viewport.x = 0.0f;
	// Set viewport y to 0
	m_Viewport.y = 0.0f;
	// Set min depth to 0
	m_Viewport.minDepth = 0.0f;
	// Set max depth to 1
	m_Viewport.maxDepth = 1.0f;

	// Set offset to 0, 0
	m_Scissor.offset = { 0, 0 };
}

void D3D::Viewport::SetViewport(VkCommandBuffer commandBuffer, VkExtent2D extent)
{
	// Set the viewport width to the width of the swapchain
	m_Viewport.width = static_cast<float>(extent.width);
	// Set the viewport width to the width of the swapchain
	m_Viewport.height = static_cast<float>(extent.height);

	// Set the viewport
	vkCmdSetViewport(commandBuffer, 0, 1, &m_Viewport);

	
	// Set scissor extent to swapchain extent
	m_Scissor.extent = extent;

	// Set the scissor
	vkCmdSetScissor(commandBuffer, 0, 1, &m_Scissor);
}
