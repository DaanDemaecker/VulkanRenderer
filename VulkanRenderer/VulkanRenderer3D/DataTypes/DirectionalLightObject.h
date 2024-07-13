// DirectionalLightObject.h
// This class will hold all the info, buffers and memory for a single directional light

#ifndef DirectionalLightObjectIncluded
#define DirectionalLightObjectIncluded

// File includes
#include "Includes/GLFWIncludes.h"
#include "Includes/GLMIncludes.h"
#include "Structs.h"
#include "DescriptorObjects/UboDescriptorObject.h"

namespace D3D
{
	// Class declaration for vulkan renderer
	class VulkanRenderer3D;

	class DirectionalLightObject
	{
	public:
		// Default constructor
		DirectionalLightObject();
		
		// Default destructor
		~DirectionalLightObject() = default;

		// Delete copy and move functions
		DirectionalLightObject(DirectionalLightObject& other) = delete;
		DirectionalLightObject(DirectionalLightObject&& other) = delete;
		DirectionalLightObject& operator=(DirectionalLightObject& other) = delete;
		DirectionalLightObject& operator=(DirectionalLightObject&& other) = delete;
		
		// Function for cleaning up allocated memory
		// Parameters:
		//     device: handle of the VkDevice object
		void Cleanup(VkDevice device);
		
		// Function for updating the vulkan buffers
		// Parameters:
		//     frame: which frame in flight it currently is
		void UpdateBuffer(int frame);

		// Set the direction of the light
		// Parameters:
		//     direction: vector for direction of the light
		void SetDirection(glm::vec3& direction);
		
		// Set the direction of the light with R-value
		// Parameters:
		//     direction: vector for direction of the light
		void SetDirection(glm::vec3&& direction);
		
		// Set the color of the light
		// Parameters:
		//     direction: vector for the color
		void SetColor(glm::vec3& color);

		// Set the color of the light with R-value
		// Parameters:
		//     direction: vector for the color
		void SetColor(glm::vec3&& color);

		// Set the intensity of the light
		// Parameters:
		//     intensity: the new intensity of the light
		void SetIntensity(float intensity);
		
		// Public getter for the vulkan buffers needed for the shader
		DescriptorObject* GetDescriptorObject();

		// Public getter for the vulkan buffers needed for the shader
		DescriptorObject* GetTransformDescriptorObject();


		// Public getter to get the struct that holds the values
		const DirectionalLightStruct& GetLight() const { return m_BufferObject; }

		glm::mat4& GetLightMatrix();
	private:
		float m_ClippingDistance{ 100.f };
		// Sttruct that holds the values of the light
		DirectionalLightStruct m_BufferObject{};
		
		// Vector for dirty flags
		std::vector<bool> m_LightChanged{};
		
		std::unique_ptr<UboDescriptorObject<DirectionalLightStruct>> m_DescriptorObject{};
		std::unique_ptr<UboDescriptorObject<glm::mat4>> m_LightMatrixDescriptorObject{};
		
		glm::mat4 m_LightTransform{};

		glm::vec4 m_OrthoBorders{ -25, 25, -25, 25 };

		// Function for creating the buffers
		void CreateLightBuffer();
		
		// Function to set all dirty flags
		void SetDirtyFlags();

		void CalculateLightTransform(int frame);
	};
}


#endif // !DirectionalLightObjectIncluded