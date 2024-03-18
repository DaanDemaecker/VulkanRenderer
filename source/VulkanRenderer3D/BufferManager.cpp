// BufferManager.cpp

// File includes
#include "BufferManager.h"
#include "GPUObject.h"
#include "VulkanUtils.h"
#include "CommandpoolManager.h"
#include "GPUObject.h"
#include "CommandpoolManager.h"

// Standard library includes
#include <stdexcept>

void D3D::BufferManager::CreateBuffer(D3D::GPUObject* pGPUObject, VkDeviceSize size,
	VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	// Get device
	auto device{ pGPUObject->GetDevice() };

	// Create buffer create info
	VkBufferCreateInfo bufferInfo{};
	// Set type to buffer create info
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	// Set size to given size
	bufferInfo.size = size;
	// Set usage to given usage
	bufferInfo.usage = usage;
	// Set sharing mode to exclusive
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	// Create buffer
	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to create buffer!");
	}

	// Create memory requiremnts object
	VkMemoryRequirements memRequirements;
	// Get the memory requirements
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	// Create memory allocation info
	VkMemoryAllocateInfo allocInfo{};
	// Set type to memory allocate info
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	// Set size to memory requirements size
	allocInfo.allocationSize = memRequirements.size;
	// Find a memory type that satisfies the requested properties
	allocInfo.memoryTypeIndex = VulkanUtils::FindMemoryType(pGPUObject->GetPhysicalDevice(), memRequirements.memoryTypeBits, properties);

	// Allocate the memory
	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	// Bind the buffer to the buffer memory
	vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void D3D::BufferManager::CopyBuffer(D3D::GPUObject* pGPUObject, D3D::CommandpoolManager* pCommandPoolManager, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	// Begin a single time command buffer
	VkCommandBuffer commandBuffer = pCommandPoolManager->BeginSingleTimeCommands(pGPUObject->GetDevice());

	// Create a buffer copy region
	VkBufferCopy copyRegion{};
	// Set size to the size of the buffer
	copyRegion.size = size;

	// Copy the buffer
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	// End the single time command
	pCommandPoolManager->EndSingleTimeCommands(pGPUObject, commandBuffer);
}

void D3D::BufferManager::CreateVertexBuffer(D3D::GPUObject* pGPUObject, D3D::CommandpoolManager* pCommandPoolManager, std::vector<D3D::Vertex>& vertices, VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory)
{
	auto device{ pGPUObject->GetDevice() };

	// Calculate buffer size for vertices
	VkDeviceSize bufferSize = sizeof(D3D::Vertex) * vertices.size();

	// Create staging buffer
	VkBuffer stagingBuffer;
	// Create staging buffer memory
	VkDeviceMemory stagingBufferMemory;

	// Create buffer
	CreateBuffer(pGPUObject, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	// Create void pointer for data
	void* data;

	// Map memory of data to stagingbuffermemory
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	// Copy data from vertices to data pointer
	memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
	// Unmap memory
	vkUnmapMemory(device, stagingBufferMemory);

	// Create the buffer
	CreateBuffer(pGPUObject, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

	// Copy buffer
	CopyBuffer(pGPUObject, pCommandPoolManager, stagingBuffer, vertexBuffer, bufferSize);

	// Destroy staging buffer
	vkDestroyBuffer(device, stagingBuffer, nullptr);
	// Free staging buffer memory
	vkFreeMemory(device, stagingBufferMemory, nullptr);

}

void D3D::BufferManager::CreateIndexBuffer(D3D::GPUObject* pGPUObject, D3D::CommandpoolManager* pCommandPoolManager, std::vector<uint32_t>& indices, VkBuffer& indexBuffer, VkDeviceMemory& indexBufferMemory)
{
	auto device{ pGPUObject->GetDevice() };

	// Calculate buffer size for indices
	VkDeviceSize bufferSize = sizeof(uint32_t) * indices.size();

	// Create staging buffer
	VkBuffer stagingBuffer;
	// Create staging buffer memory
	VkDeviceMemory stagingBufferMemory;
	// Create buffer
	CreateBuffer(pGPUObject, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	// Create void pointer for data
	void* data;
	// Map memory of data to staging buffer memory
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	// Copy memory from indices to data
	memcpy(data, indices.data(), static_cast<size_t>(bufferSize));

	// Create buffer
	CreateBuffer(pGPUObject, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	// Copy staging buffer to index buffer
	CopyBuffer(pGPUObject, pCommandPoolManager, stagingBuffer, indexBuffer, bufferSize);

	// Destroy staging buffer
	vkDestroyBuffer(device, stagingBuffer, nullptr);

	// Free staging buffer memory
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}