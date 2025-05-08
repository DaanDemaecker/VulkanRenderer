// Mesh.cpp

// Header include
#include "Mesh.h"

// File includes
#include "Vulkan/Vulkan3D.h"
#include "Vulkan/Wrappers/PipelineWrapper.h"
#include "Engine/DDMModelLoader.h"
#include "Utils/Utils.h"
#include "DDMModelLoader/Mesh.h"

// Standard library includes
#include <algorithm>

DDM3::Mesh::Mesh(const std::string& filePath)
{
	// Load the vertices and indices
	DDM3::DDMModelLoader::GetInstance().LoadModel(filePath, m_Vertices, m_Indices);

	SetupBuffers();
}

DDM3::Mesh::Mesh(const std::vector<DDM3::Vertex>& vertices, const std::vector<uint32_t>& indices)
{
	m_Vertices.clear();
	m_Indices.clear();

	m_Vertices.resize(vertices.size());
	m_Indices.resize(indices.size());

	std::copy(vertices.begin(), vertices.end(), m_Vertices.begin());
	std::copy(indices.begin(), indices.end(), m_Indices.begin());


	SetupBuffers();
}

DDM3::Mesh::Mesh(DDMML::Mesh& ddmmlMesh)
{
	m_Vertices.clear();
	m_Indices.clear();

	auto& vertices = ddmmlMesh.GetVertices();
	auto& indices = ddmmlMesh.GetIndices();

	DDMModelLoader::GetInstance().ConvertVertices(vertices, m_Vertices);

	m_Indices.resize(indices.size());

	std::copy(indices.begin(), indices.end(), m_Indices.begin());


	SetupBuffers();
}


DDM3::Mesh::~Mesh()
{
	// Call cleanup function
	Cleanup();
}

void DDM3::Mesh::Render(VkCommandBuffer commandBuffer)
{
	// If not initialized, don't render
	if (!m_Initialized)
		return;

	// Set and bind vertex buffer
	VkBuffer vertexBuffers[] = { m_VertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	// Bind index buffer
	vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);

	
	// Draw
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
}

void DDM3::Mesh::Cleanup()
{
	// Get handle of device
	auto device = DDM3::Vulkan3D::GetInstance().GetDevice();

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

void DDM3::Mesh::SetupBuffers()
{
	// Get reference to the renderer
	auto& renderer{ Vulkan3D::GetInstance().GetRenderer() };

	// Create vertex and index buffer
	m_Initialized =  renderer.CreateVertexBuffer(m_Vertices, m_VertexBuffer, m_VertexBufferMemory) &&
	renderer.CreateIndexBuffer(m_Indices, m_IndexBuffer, m_IndexBufferMemory);


}