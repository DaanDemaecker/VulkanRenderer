// BufferManager.h
// This file wil handle buffer creation, buffer copying and everything else to do with buffers

#ifndef BufferManagerIncluded
#define BufferManagerIncluded

// File includes
#include "Includes/VulkanIncludes.h"
#include "DataTypes/Structs.h"

// Standard library includes
#include <vector>

namespace DDM3
{
	// Class forward declaratoin
	class GPUObject;
	class CommandpoolManager;

	class BufferManager final
	{
	public:
		// Default constructor
		BufferManager() = default;

		// Default destructor
		~BufferManager() = default;

		// Delete copy and move functions
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
		void CreateBuffer(DDM3::GPUObject* pGPUObbject, VkDeviceSize size,
			VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

		// Copy a buffer to another buffer
		// Parameters:
		//     pGPUObject: pointer to the GPU object
		//     pCommandPoolManager: pointer to the Command Pool Manager
		//     srcBuffer: the source buffer
		//     dstBuffer: the destination buffer
		//     size: the size of the buffers
		void CopyBuffer(DDM3::GPUObject* pGPUObject, DDM3::CommandpoolManager* pCommandPoolManager, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

		// Create a vertex buffer
		// Parameters:
		// Copy a buffer to another buffer
		// Parameters:
		//     pGPUObject: pointer to the GPU object
		//     pCommandPoolManager: pointer to the Command Pool Manager
		//     vertices: a vector of vertex objects to store in a buffer
		//     vertexBuffer: handle of the vkBuffer to store the vertices in
		//     vertexBufferMemory: handle of the VkDeviceMemory object
		void CreateVertexBuffer(DDM3::GPUObject* pGPUObject, DDM3::CommandpoolManager* pCommandPoolManager,
			std::vector<DDM3::Vertex>& vertices, VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory);

		// Create a vertex buffer
		// Parameters:
		// Copy a buffer to another buffer
		// Parameters:
		//     pGPUObject: pointer to the GPU object
		//     pCommandPoolManager: pointer to the Command Pool Manager
		//     indices: a vector of indices to store in a buffer
		//     indexBuffer: handle of the vkBuffer to store the indices in
		//     indexBufferMemory: handle of the VkDeviceMemory object
		void CreateIndexBuffer(DDM3::GPUObject* pGPUObject, DDM3::CommandpoolManager* pCommandPoolManager,
			std::vector<uint32_t>& indices, VkBuffer& indexBuffer, VkDeviceMemory& indexBufferMemory);
	};
}
#endif // !BufferManagerIncluded
