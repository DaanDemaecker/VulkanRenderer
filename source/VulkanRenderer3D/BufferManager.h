// BufferManager.h
// This file wil handle buffer creation, buffer copying and everything else to do with buffers

#ifndef BufferManagerIncluded
#define BufferManagerIncluded

// File includes
#include "VulkanIncludes.h"
#include "Structs.h"

// Standard library includes
#include <vector>

namespace D3D
{
	// Class forward declaratoin
	class GPUObject;
	class CommandpoolManager;

	class BufferManager final
	{
	public:
		BufferManager() = default;

		~BufferManager() = default;

		BufferManager(BufferManager& other) = delete;
		BufferManager(BufferManager&& other) = delete;

		BufferManager& operator=(BufferManager& other) = delete;
		BufferManager& operator=(BufferManager&& other) = delete;

		// Create a VkBuffer
		// Parameters:
		//     pGPUObject : a pointer to the GPU object 
		//     size: the size of the buffer
		//     usage: the usage flags for the buffer
		//     properties: the property flags for the buffer
		//     buffer: a reference to the buffer that will be created
		//     bufferMemory: a reference to the memory for the buffer that will be created
		void CreateBuffer(D3D::GPUObject* pGPUObbject, VkDeviceSize size,
			VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

		// Copy a buffer to another buffer
		// Parameters:
		//     pGPUObject: pointer to the GPU object
		//     pCommandPoolManager: pointer to the Command Pool Manager
		//     srcBuffer: the source buffer
		//     dstBuffer: the destination buffer
		//     size: the size of the buffers
		void CopyBuffer(D3D::GPUObject* pGPUObject, D3D::CommandpoolManager* pCommandPoolManager, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

		void CreateVertexBuffer(D3D::GPUObject* pGPUObject, D3D::CommandpoolManager* pCommandPoolManager,
			std::vector<D3D::Vertex>& vertices, VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory);

		void CreateIndexBuffer(D3D::GPUObject* pGPUObject, D3D::CommandpoolManager* pCommandPoolManager,
			std::vector<uint32_t>& indices, VkBuffer& indexBuffer, VkDeviceMemory& indexBufferMemory);

		
	};
}
#endif // !BufferManagerIncluded
