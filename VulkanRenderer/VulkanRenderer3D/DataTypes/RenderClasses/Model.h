// Model.h
// This classed is used to load in and render 3D models

#ifndef ModelIncluded
#define ModelIncluded

// File includes
#include "Includes/GLFWIncludes.h"
#include "Includes/GLMIncludes.h"

#include "DataTypes/Structs.h"
#include "DataTypes/DescriptorObjects/UboDescriptorObject.h"

// Standard library includes
#include <memory>
#include <iostream>

namespace DDM3
{
	// Class forward declarations
	class Material;
	class PipelineWrapper;
	class Mesh;

	class Model final : public std::enable_shared_from_this<Model>
	{
	public:
		// Constructor
		Model();
		// Destructor
		~Model();

		// Delete copy and move functions
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

		void RenderShadow(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);

		// Render model
		void Render();

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

		void SetRotate(bool rotate) { m_Rotate = rotate; }
		void SetCastsShadow(bool shouldCast) { m_CastsShadow = shouldCast; }
	private:
		bool m_Rotate{true};
		bool m_CastsShadow{ true };

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

		std::unique_ptr<DDM3::UboDescriptorObject<UniformBufferObject>> m_pUboDescriptorObject{};

		// Vector of descriptorsets
		std::vector<VkDescriptorSet> m_DescriptorSets{};

		//Mesh
		std::unique_ptr<DDM3::Mesh> m_pMesh{};

		//Material
		std::shared_ptr<Material> m_pMaterial{};

		// Create uniform buffer
		void CreateUniformBuffers();

		// UPdate descriptorsets
		void UpdateDescriptorSets();

		// Update uniform buffers
		// Parameters:
		//     frame: index of current frame
		void UpdateUniformBuffer(uint32_t frame);

		// Get the pipeline that the material is bound to
		PipelineWrapper* GetPipeline();

		// CLeanup
		void Cleanup();

		// Set dirty flags for UBOs
		void SetDirtyFlags();
	};
}

#endif // !ModelIncluded