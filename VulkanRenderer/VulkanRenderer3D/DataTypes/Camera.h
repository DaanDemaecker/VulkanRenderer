// Camera.h
// This class will serve as a camera object and store the values of location, rotation and scale of the camera

#ifndef CameraIncluded
#define CameraIncluded

// File includes
#include "Includes/GLMIncludes.h"
#include "Structs.h"

namespace D3D
{
	enum class CameraType
	{
		Perspective,
		Ortographic
	};

	class Camera
	{
	public:
		// Default constructor
		Camera();

		// Defualt destructor
		~Camera() = default;
		
		// Delete copy and move functions
		Camera(Camera& other) = delete;
		Camera(Camera&& other) = delete;
		Camera& operator=(Camera& other) = delete;
		Camera& operator=(Camera&& other) = delete;

		void Update();

		// Set position
		// Parameters:
		//     position: new position
		void SetPosition(glm::vec3& position) { m_Position = position; SetDirtyFlag(); }

		// Set position
		// Parameters:
		//     position: new position
		void SetPosition(glm::vec3&& position) { m_Position = position; SetDirtyFlag(); }

		// Set position
		// Parameters:
		//     x: x-position
		//     y: y-position
		//     z: z-position
		void SetPosition(float x, float y, float z) { SetPosition(glm::vec3{ x,y,z }); SetDirtyFlag();}

		// Set rotation
		// Parameters:
		//     rotation: new rotation
		void SetRotation(glm::vec3& rotation) { m_Rotation = glm::quat(rotation); SetDirtyFlag();}
		// Set rotation
		// Parameters:
		//     rotation: new rotation
		void SetRotation(glm::vec3&& rotation);
		// Set rotation
		// Parameters:
		//     x: x-rotation
		//     y: y-rotation
		//     z: z-rotation
		void SetRotation(float x, float y, float z) { SetRotation(glm::vec3{ x,y,z });}

		void SetDirection(glm::vec3& direction);

		void SetDirection(glm::vec3&& direction) { SetDirection(direction); }

		void SetDirection(float x, float y, float z) { SetDirection(glm::vec3{ x,y,z }); }

		// Set scale
		// Parameters:
		//     scale: new scale
		void SetScale(glm::vec3& scale) { m_Scale = scale; SetDirtyFlag();}
		// Set scale
		// Parameters:
		//     scale: new scale
		void SetScale(glm::vec3&& scale) { m_Scale = scale; SetDirtyFlag();}
		// Set scale
		// Parameters:
		//     x: x-scale
		//     y: y-scale
		//     z: z-scale
		void SetScale(float x, float y, float z) { SetScale(glm::vec3{ x,y,z }); SetDirtyFlag();}

		// Set fov angle in radians
		// Parameters:
		//     angle: the angle in radians
		void SetFovAngle(float angle) { m_FovAngle = angle; SetDirtyFlag();}

		// Set fov angle in degrees
		// Parameters:
		//     angle: the angle in degrees
		void SetFovAngleDegrees(float angle) { m_FovAngle = glm::radians(angle); SetDirtyFlag();}

		// Get the position of the camera
		const glm::vec3& GetPosition() const {return m_Position; }
		// Get the rotation of the camera
		const glm::quat& GetRotation() const {return m_Rotation; }
		// Get the scale of the camera
		const glm::vec3& GetScale() const { return m_Scale; }

		// Update uniform buffer with camera transform
		// Parameters:
		//     buffer: reference to the uniform buffer object that needs updating
		//     extent: the extent of the swapchain
		void UpdateUniformBuffer(UniformBufferObject& buffer, VkExtent2D extent);

		glm::vec3 GetForward();

		glm::vec3 GetRight();

		glm::vec3 GetUp();

		void SetCameraType(CameraType type) { m_Type = type; SetDirtyFlag(); }
		CameraType GetCameraType() const { return m_Type; }

		void SetOrthographicBorders(glm::vec4&& borders) { SetOrthographicBorders(borders); }
		void SetOrthographicBorders(glm::vec4& borders) { m_OrthoBorders = borders; SetDirtyFlag(); }
		void SetOrthographicBorders(float left, float right, float top, float bottom) { SetOrthographicBorders(glm::vec4{ left, right, top, bottom }); }

		glm::vec4 GetOrtographicBorders() const { return m_OrthoBorders; }
	private:
		CameraType m_Type{ CameraType::Perspective };

		const float m_Speed{ 5.f };
		const float m_AngularSpeed{ 0.5f };

		float m_TotalPitch{0};
		float m_TotalYaw{0};

		double m_PrevXPos{};
		double m_PrevYPos{};

		// Dirty flag
		bool m_HasChanged{ true };

		//FovAgnle
		float m_FovAngle{ static_cast<float>(glm::radians(90.0)) };
		
		glm::vec4 m_OrthoBorders{ -25, 25, -25, 25 };

		// Vector 3 for the position
		glm::vec3 m_Position{0, 0, 0};

		// Quaternion for the rotation
		glm::quat m_Rotation{};

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

#endif // !CameraIncluded