// Mesh.cpp

// File includes
#include "Mesh.h"
#include "VulkanRenderer3D.h"
#include "PipelineWrapper.h"
#include "Utils.h"

D3D::Mesh::Mesh(const std::string& filePath)
{
	// Load the vertices and indices
	Utils::LoadModel(filePath, m_Vertices, m_Indices);

	// Get reference to the renderer
	auto& renderer{ VulkanRenderer3D::GetInstance() };

	// Create vertex and index buffer
	renderer.CreateVertexBuffer(m_Vertices, m_VertexBuffer, m_VertexBufferMemory);
	renderer.CreateIndexBuffer(m_Indices, m_IndexBuffer, m_IndexBufferMemory);
}

D3D::Mesh::~Mesh()
{
	// Call cleanup function
	Cleanup();
}

void D3D::Mesh::Render(PipelineWrapper* pPipeline, VkDescriptorSet* descriptorSet)
{
	// Get current commandbuffer
	auto commandBuffer{ VulkanRenderer3D::GetInstance().GetCurrentCommandBuffer() };

	// Bind pipeline
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->GetPipeline());

	// Set and bind vertex buffer
	VkBuffer vertexBuffers[] = { m_VertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	// Bind index buffer
	vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);

	// Bind descriptor sets
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->GetPipelineLayout(), 0, 1, descriptorSet, 0, nullptr);

	// Draw
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
}

void D3D::Mesh::Cleanup()
{
	// Get reference to renderer
	auto& renderer = D3D::VulkanRenderer3D::GetInstance();
	// Get reference to device
	auto device = renderer.GetDevice();

	// Wait until device is idle
	vkDeviceWaitIdle(device);

	// Destroy index buffer
	vkDestroyBuffer(device, m_IndexBuffer, nullptr);
	// Free index buffer memory
	vkFreeMemory(device, m_IndexBufferMemory, nullptr);

	// Destroy vertex buffer
	vkDestroyBuffer(device, m_VertexBuffer, nullptr);
	// Free vertex buffer
	vkFreeMemory(device, m_VertexBufferMemory, nullptr);
}
