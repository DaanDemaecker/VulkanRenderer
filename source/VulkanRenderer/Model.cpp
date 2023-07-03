#include "stdafx.h"
#include "Model.h"
#include "VulkanHelpers.h"
#include "VulkanRenderer.h"
#include "Utils.h"

D3D::Model::Model()
{
	Utils::LoadModel(m_Vertices, m_Indices);
	CreateVertexBuffer();
	CreateIndexBuffer();
}

D3D::Model::~Model()
{
	auto device = D3D::VulkanRenderer::GetInstance().GetDevice();

	vkDestroyBuffer(device, m_IndexBuffer, nullptr);
	vkFreeMemory(device, m_IndexBufferMemory, nullptr);

	vkDestroyBuffer(device, m_VertexBuffer, nullptr);
	vkFreeMemory(device, m_VertexBufferMemory, nullptr);
}

void D3D::Model::CreateVertexBuffer()
{
	auto& renderer{ D3D::VulkanRenderer::GetInstance() };
	auto device = renderer.GetDevice();

	VkDeviceSize bufferSize = sizeof(m_Vertices[0]) * m_Vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	renderer.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;

	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, m_Vertices.data(), static_cast<size_t>(bufferSize));
	vkUnmapMemory(device, stagingBufferMemory);

	renderer.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VertexBuffer, m_VertexBufferMemory);

	renderer.CopyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void D3D::Model::CreateIndexBuffer()
{
	auto& renderer{ D3D::VulkanRenderer::GetInstance() };
	auto device = renderer.GetDevice();

	VkDeviceSize bufferSize = sizeof(m_Indices[0]) * m_Indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	renderer.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, m_Indices.data(), static_cast<size_t>(bufferSize));

	renderer.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_IndexBuffer, m_IndexBufferMemory);

	renderer.CopyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}
