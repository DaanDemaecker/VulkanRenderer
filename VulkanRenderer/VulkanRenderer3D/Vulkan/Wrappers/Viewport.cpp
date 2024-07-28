// Viewport.cpp

// File includes
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

D3D::Viewport::Viewport(VkExtent2D viewportSize)
{
	SetViewportAndScissor(viewportSize);
}

void D3D::Viewport::SetViewportSize(VkExtent2D viewportSize)
{
	m_Viewport.width = static_cast<float>(viewportSize.width);
	m_Viewport.height = static_cast<float>(viewportSize.height);
}

void D3D::Viewport::SetScissor(VkExtent2D extent)
{
	m_Scissor.extent = extent;

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

void D3D::Viewport::SetViewport(VkCommandBuffer commandBuffer)
{
	// Set the viewport
	vkCmdSetViewport(commandBuffer, 0, 1, &m_Viewport);

	// Set the scissor
	vkCmdSetScissor(commandBuffer, 0, 1, &m_Scissor);
}

void D3D::Viewport::SetViewportAndScissor(VkExtent2D size)
{
	SetViewportSize(size);

	SetScissor(size);
}
