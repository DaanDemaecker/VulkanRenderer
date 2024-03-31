// DirectionalLightObject.h
// This class will hold all the info, buffers and memory for a single directional light

#ifndef DirectionalLightObjectIncluded
#define DirectionalLightObjectIncluded

// File includes
#include "GLFWIncludes.h"
#include "Structs.h"
#include "GLMIncludes.h"
#include "UboDescriptorObject.h"

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

		// Public getter to get the struct that holds the values
		const DirectionalLightStruct& GetLight() const { return m_BufferObject; }

	private:
		// Sttruct that holds the values of the light
		DirectionalLightStruct m_BufferObject{};
		
		// Vector for dirty flags
		std::vector<bool> m_LightChanged{};
		
		std::unique_ptr<UboDescriptorObject<DirectionalLightStruct>> m_DescriptorObject{};
		

		// Function for creating the buffers
		void CreateLightBuffer();
		
		// Function to set all dirty flags
		void SetDirtyFlags();
	};
}


#endif // !DirectionalLightObjectIncluded