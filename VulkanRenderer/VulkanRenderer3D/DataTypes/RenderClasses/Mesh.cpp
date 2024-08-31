// Mesh.cpp

// Header include
#include "Mesh.h"

// File includes
#include "Vulkan/Vulkan3D.h"
#include "Vulkan/Wrappers/PipelineWrapper.h"

#include "Utils/Utils.h"

D3D::Mesh::Mesh(const std::string& filePath)
{
	// Load the vertices and indices
	Utils::LoadModel(filePath, m_Vertices, m_Indices);

	// Get reference to the renderer
	auto& renderer{ Vulkan3D::GetInstance().GetRenderer()};

	// Create vertex and index buffer
	renderer.CreateVertexBuffer(m_Vertices, m_VertexBuffer, m_VertexBufferMemory);
	renderer.CreateIndexBuffer(m_Indices, m_IndexBuffer, m_IndexBufferMemory);
}

D3D::Mesh::~Mesh()
{
	// Call cleanup function
	Cleanup();
}

void D3D::Mesh::Render(VkCommandBuffer commandBuffer)
{
	// Set and bind vertex buffer
	VkBuffer vertexBuffers[] = { m_VertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	// Bind index buffer
	vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);

	
	// Draw
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
}

void D3D::Mesh::Cleanup()
{
	// Get handle of device
	auto device = D3D::Vulkan3D::GetInstance().GetDevice();

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
