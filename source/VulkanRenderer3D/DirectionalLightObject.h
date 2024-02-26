// DirectionalLightObject.h
// This class will hold all the info, buffers and memory for a single directional light

#ifndef DirectionalLightObjectIncluded
#define DirectionalLightObjectIncluded

// File includes
#include "GLFWIncludes.h"
#include "Structs.h"
#include "GLMIncludes.h"

namespace D3D
{
	// Class declaration for vulkan renderer
	class VulkanRenderer3D;

	class DirectionalLightObject
	{
	public:
		//Constructor
		// This constructor will only be called from in the renderer itsel
		// Parameters:
		//     renderer: a pointer to the vulkanRenderer
		DirectionalLightObject(VulkanRenderer3D* renderer);
		
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
		std::vector<VkBuffer>& GetLightBuffers() { return m_LightBuffers; }

		// Public getter to get the struct that holds the values
		const DirectionalLightStruct& GetLight() const { return m_BufferObject; }

	private:
		// Sttruct that holds the values of the light
		DirectionalLightStruct m_BufferObject{};
		
		// Vector for dirty flags
		std::vector<bool> m_LightChanged{};
		
		// Vector with buffers for the data
		std::vector<VkBuffer> m_LightBuffers{};

		// Vector with device memories
		std::vector<VkDeviceMemory> m_LightMemory{};

		// Vector with mapped memory of light
		std::vector<void*> m_LightMapped{};

		// Function for creating the buffers
		// Parameters:
		//     renderer: pointer to vulkan renderer
		void CreateLightBuffer(VulkanRenderer3D* renderer);
		
		// Function to set all dirty flags
		void SetDirtyFlags();
	};
}


#endif // !DirectionalLightObjectIncluded