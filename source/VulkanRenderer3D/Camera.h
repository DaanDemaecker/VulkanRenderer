// Camera.h
// This class will serve as a camera object and store the values of location, rotation and scale of the camera

// File includes
#include "GLMIncludes.h"
#include "Structs.h"

namespace D3D
{
	class Camera
	{
	public:
		// Default constructor
		Camera() = default;

		// Defualt destructor
		~Camera() = default;


		// Set position
		// Parameters:
		//     position: new position
		void SetPosition(glm::vec3& position) { m_Position = position; }

		// Set position
		// Parameters:
		//     position: new position
		void SetPosition(glm::vec3&& position) { m_Position = position; }

		// Set position
		// Parameters:
		//     x: x-position
		//     y: y-position
		//     z: z-position
		void SetPosition(float x, float y, float z) { SetPosition(glm::vec3{ x,y,z }); }

		// Set rotation
		// Parameters:
		//     rotation: new rotation
		void SetRotation(glm::vec3& rotation) { m_Rotation = rotation; }
		// Set rotation
		// Parameters:
		//     rotation: new rotation
		void SetRotation(glm::vec3&& rotation) { m_Rotation = rotation; }
		// Set rotation
		// Parameters:
		//     x: x-rotation
		//     y: y-rotation
		//     z: z-rotation
		void SetRotation(float x, float y, float z) { SetRotation(glm::vec3{ x,y,z }); }

		// Set scale
		// Parameters:
		//     scale: new scale
		void SetScale(glm::vec3& scale) { m_Scale = scale; }
		// Set scale
		// Parameters:
		//     scale: new scale
		void SetScale(glm::vec3&& scale) { m_Scale = scale; }
		// Set scale
		// Parameters:
		//     x: x-scale
		//     y: y-scale
		//     z: z-scale
		void SetScale(float x, float y, float z) { SetScale(glm::vec3{ x,y,z }); }

		// Get the position of the camera
		glm::vec3& GetPosition() { SetDirtyFlag(); return m_Position; }
		// Get the rotation of the camera
		glm::vec3& GetRotation() { SetDirtyFlag(); return m_Rotation; }
		// Get the scale of the camera
		glm::vec3& GetScale() { SetDirtyFlag(); return m_Scale; }

		// Update uniform buffer with camera transform
		// Parameters:
		//     buffer: reference to the uniform buffer object that needs updating
		void UpdateUniformBuffer(UniformBufferObject& buffer);

	private:
		// Dirty flag
		bool m_HasChanged{ true };

		// Vector 3 for the position
		glm::vec3 m_Position{0, 0, 0};

		// Vector 3 for the rotation
		glm::vec3 m_Rotation{0, 0, 0};

		// Vector 3 for the scale
		glm::vec3 m_Scale{1, 1, 1};

		// Matrix 4 for the complete transform
		glm::mat4 m_Matrix{};

		// Set the dirty flag
		void SetDirtyFlag() { m_HasChanged = true; }

		// Update the transform matrix
		void UpdateMatrix();
	};
}
