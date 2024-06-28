#ifndef MeshIncluded
#define MeshIncluded

// File includes
#include "VulkanIncludes.h"
#include "Structs.h"

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
		//     pPipeline: the pipeline to bind before drawing
		//     descriptorSet: the descriptorset to bind before drawing
		void Render(PipelineWrapper* pPipeline, VkDescriptorSet* descriptorSet);
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