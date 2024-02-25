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
	class VulkanRenderer3D;

	class DirectionalLightObject
	{
	public:
		DirectionalLightObject(VulkanRenderer3D* renderer);

		DirectionalLightObject();

		~DirectionalLightObject() = default;

		DirectionalLightObject(DirectionalLightObject& other) = delete;
		DirectionalLightObject(DirectionalLightObject&& other) = delete;

		DirectionalLightObject& operator=(DirectionalLightObject& other) = delete;
		DirectionalLightObject& operator=(DirectionalLightObject&& other) = delete;

		void Cleanup(VkDevice device);

		void UpdateBuffer(int frame);

		void CreateLightBuffer(VulkanRenderer3D* renderer);
		void CreateLightBuffer();

		void SetDirection(glm::vec3& direction);
		void SetColor(glm::vec3& color);
		void SetIntensity(float intensity);

		void SetDirection(glm::vec3&& direction);
		void SetColor(glm::vec3&& color);

		std::vector<VkBuffer>& GetLightBuffers() { return m_LightBuffers; }
		const DirectionalLightStruct& GetLight() const { return m_BufferObject; }

	private:
		DirectionalLightStruct m_BufferObject{};

		std::vector<bool> m_LightChanged{};

		std::vector<VkBuffer> m_LightBuffers{};
		std::vector<VkDeviceMemory> m_LightMemory{};
		std::vector<void*> m_LightMapped{};

		void SetDirtyFlags();
	};
}


#endif // !DirectionalLightObjectIncluded