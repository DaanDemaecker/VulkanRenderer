#ifndef MeshIncluded
#define MeshIncluded

// File includes
#include "Includes/VulkanIncludes.h"

#include "DataTypes/Structs.h"

// Standard library includes
#include <string>
#include <vector>

namespace DDMML
{
	class Mesh;
}

namespace DDM3
{
	// Class forward declarations
	class ResourceManager;
	class PipelineWrapper;

	class Mesh final
	{
	public:
		/// Constructor, loads mesh from given filepath
		///
		/// \param     filePath: the filepath to the 3D model
		Mesh(const std::string& filePath);

		/// <summary>
		/// Constructor, loads mesh with given vertex and index list
		/// </summary>
		/// <param name="vertices">List of vertices</param>
		/// <param name="indices">List of indices</param>
		Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

		/// <summary>
		/// Constructor, loads in mesh with mesh class from DDMModelLoader
		/// </summary>
		/// <param name="ddmmlMesh: ">DDMModelLoader mesh</param>
		Mesh(DDMML::Mesh& ddmmlMesh);

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

		/// <summary>
		/// Sets up vertex and index buffers
		/// </summary>
		void SetupBuffers();
	};
}

#endif // !MeshIncluded