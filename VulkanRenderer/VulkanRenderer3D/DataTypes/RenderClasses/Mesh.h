#ifndef MeshIncluded
#define MeshIncluded

// File includes
#include "Includes/VulkanIncludes.h"
#include "DataTypes/Structs.h"

// Standard library includes
#include <string>
#include <vector>

namespace D3D
{
	// Class forward declarations
	class ResourceManager;
	class PipelineWrapper;

	class Mesh final
	{
	public:
		// Constructor
		// Parameters:
		//     filePath: the filepath to the 3D model
		Mesh(const std::string& filePath);

		// Delete default constructor
		Mesh() = delete;

		// Destructor
		~Mesh();

		// Render the model
		// Parameters:
		//     -commandBuffer: the commandbuffer used in this renderpass
		void Render(VkCommandBuffer commandBuffer);
	private:
		// Vector of vertices
		std::vector<Vertex> m_Vertices{};

		// Vertex buffer
		VkBuffer m_VertexBuffer{};

		// Vertex buffer memory
		VkDeviceMemory m_VertexBufferMemory{};

		// Vector of indices
		std::vector<uint32_t> m_Indices{};

		// Index buffer
		VkBuffer m_IndexBuffer{};

		// Index buffer memory
		VkDeviceMemory m_IndexBufferMemory{};

		// Clean up all allocated objects
		void Cleanup();
	};
}

#endif // !MeshIncluded