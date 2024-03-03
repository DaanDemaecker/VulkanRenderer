// Model.h
// This classed is used to load in and render 3D models

#ifndef ModelIncluded
#define ModelIncluded

// File includes
#include "GLFWIncludes.h"
#include "GLMIncludes.h"
#include "Structs.h"

// Standard library includes
#include <memory>
#include <iostream>

namespace D3D
{
	// Class forward declaration for material
	class Material;

	class Model final : public std::enable_shared_from_this<Model>
	{
	public:
		// Constructor
		Model();
		// Destructor
		~Model();

		Model(Model& other) = delete;
		Model(Model&& other) = delete;

		Model& operator=(Model& other) = delete;
		Model& operator=(Model&& other) = delete;

		// Load the model
		// Parameters:
		//     textPath: textpath to where the model is stored
		void LoadModel(const std::string& textPath);
		
		// Set the material
		// Parameters:
		//     pMaterial: the material requested
		void SetMaterial(std::shared_ptr<Material> pMaterial);

		// Update model
		void Update();

		// Render model
		void Render();

		// Get vertex buffer
		const VkBuffer& GetVertexBuffer() const { return m_VertexBuffer; }
		// Get index buffer
		const VkBuffer& GetIndexBuffer() const { return m_IndexBuffer; }
		// Get amound of indices
		size_t GetIndexAmount() const { return m_Indices.size(); }

		// Set position
		// Parameters:
		//     x: x-position
		//     y: y-position
		//     z: z-position
		void SetPosition(float x, float y, float z);

		// Set rotation
		// Parameters:
		//     x: x-angle
		//     y: y-angle
		//     z: z-angle
		void SetRotation(float x, float y, float z);

		// Set scale
		// Parameters:
		//     x: x-scale
		//     y: y-scale
		//     z: z-scale
		void SetScale(float x, float y, float z);

		// Create the descriptorsets
		void CreateDescriptorSets();

	private:
		//Is model initialized
		bool m_Initialized{ false };

		// Rotation vector
		glm::vec3 m_Rotation{};
		// Position vector
		glm::vec3 m_Position{};
		// Scale vector
		glm::vec3 m_Scale{ 1, 1, 1 };

		// Vector for Uniform Buffer Objects
		std::vector<UniformBufferObject> m_Ubos{};
		// Vector of dirty flags for UBOs
		std::vector<bool> m_UboChanged{};

		// Vector of buffers for UBOs
		std::vector<VkBuffer> m_UboBuffers{};
		// Vector for memories for UBOs
		std::vector<VkDeviceMemory> m_UbosMemory{};
		// Pointers to mapped UBOs
		std::vector<void*> m_UbosMapped{};

		// Vector of descriptorsets
		std::vector<VkDescriptorSet> m_DescriptorSets{};

		// Vector of vertices
		std::vector<Vertex> m_Vertices{};
		// Vector of indices
		std::vector<uint32_t> m_Indices{};

		// Vertex buffer
		VkBuffer m_VertexBuffer{};
		// Vertex buffer memory
		VkDeviceMemory m_VertexBufferMemory{};

		// Index buffer
		VkBuffer m_IndexBuffer{};
		// Index buffer memory
		VkDeviceMemory m_IndexBufferMemory{};

		//Material
		std::shared_ptr<Material> m_pMaterial{};

		// Create vertex buffer
		void CreateVertexBuffer();
		// Create index buffer
		void CreateIndexBuffer();
		// Create uniform buffer
		void CreateUniformBuffers();

		// UPdate descriptorsets
		void UpdateDescriptorSets();

		// Update uniform buffers
		// Parameters:
		//     frame: index of current frame
		void UpdateUniformBuffer(uint32_t frame);

		// Get the pipeline that the material is bound to
		PipelinePair& GetPipeline();

		// CLeanup
		void Cleanup();

		// Set dirty flags for UBOs
		void SetDirtyFlags();
	};
}

#endif // !ModelIncluded