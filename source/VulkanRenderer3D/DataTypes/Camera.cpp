// Camera.cpp

// File includes
#include "Camera.h"
#include "../Engine/Window.h"
#include "../Engine/TimeManager.h"
#include "../Includes/ImGuiIncludes.h"
#include "../Utils/Utils.h"


void D3D::Camera::Update()
{
	glm::vec3 direction{};

	auto window = Window::GetInstance().GetWindowStruct().pWindow;

	auto deltaTime = TimeManager::GetInstance().GetDeltaTime();

	// Determine movement direction based on key presses
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		//direction += transform->GetForward() * m_Speed * deltaTime;
		direction += GetForward();
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		//direction -= transform->GetForward() * m_Speed * deltaTime;
		direction -= GetForward();
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		//direction -= transform->GetRight() * m_Speed * deltaTime;
		direction -= GetRight();
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		//direction += transform->GetRight() * m_Speed * deltaTime;
		direction += GetRight();
	}

	glm::normalize(direction);

	direction *= m_Speed * deltaTime;


	// Translate the object based on the rotated movement direction
	SetPosition(GetPosition() + direction);

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) &&
		!ImGui::IsAnyItemActive() &&
		!ImGui::IsAnyItemHovered() &&
		!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
	{
		double deltaX = xpos - m_PrevXPos;
		double deltaY = ypos - m_PrevYPos;


		m_TotalPitch += static_cast<float>(deltaY * deltaTime * m_AngularSpeed);
		m_TotalYaw += static_cast<float>(deltaX * deltaTime * m_AngularSpeed);

		// Rotate the camera based on mouse movement
		SetRotation(m_TotalPitch, m_TotalYaw, 0);
	}

	m_PrevXPos = xpos;
	m_PrevYPos = ypos;
}

void D3D::Camera::SetDirection(glm::vec3& direction)
{
	m_Rotation = Utils::RotationFromDirection(direction);

	SetDirtyFlag();
}

void D3D::Camera::UpdateUniformBuffer(UniformBufferObject& buffer, VkExtent2D extent)
{
	// If the camera transform has changed, update matrix
	if (m_HasChanged)
		UpdateMatrix();

	// Set buffer view matrix
	buffer.view = m_Matrix;

	switch (m_Type)
	{
	case D3D::CameraType::Perspective:
		// Set the projection matrix
		buffer.proj = glm::perspective(m_FovAngle, extent.width / static_cast<float>(extent.height), 0.1f, 100.0f);
		break;
	case D3D::CameraType::Ortographic:
		buffer.proj = glm::ortho( m_OrthoBorders.x, m_OrthoBorders.y, m_OrthoBorders.z, m_OrthoBorders.w, 0.1f, 100.f);
		break;
	default:
		break;
	}

	buffer.proj[1][1] *= -1;
	buffer.proj[2][2] *= -1;
	buffer.proj[2][3] *= -1;
}

void D3D::Camera::UpdateMatrix()
{
	// Set hasChanged to false
	m_HasChanged = false;

	// Create rotation matrix
	glm::mat4 rotationMatrix = glm::mat4_cast(glm::conjugate(GetRotation()));

	// Create translation matrix
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), -GetPosition());

	// Multiply matrices (apply rotation first, then translation)
	m_Matrix = rotationMatrix * translationMatrix;
}

glm::vec3 D3D::Camera::GetForward()
{
	
	// Create the rotation matrix from the quaternion
	glm::mat4 rotationMatrix = glm::mat4_cast(GetRotation());

	// Apply the rotation to the vector using the rotation matrix
	glm::vec4 rotatedVector = rotationMatrix * glm::vec4(0.f, 0.f, 1.f, 0.0f);

	// Extract the rotated glm::vec3 from the glm::vec4
	glm::vec3 finalRotatedVector = glm::vec3(rotatedVector);

	return finalRotatedVector;
}

glm::vec3 D3D::Camera::GetUp()
{
	// Create the rotation matrix from the quaternion
	glm::mat4 rotationMatrix = glm::mat4_cast(GetRotation());

	// Apply the rotation to the vector using the rotation matrix
	glm::vec4 rotatedVector = rotationMatrix * glm::vec4(0.f, 1.f, 0.f, 0.0f);  // Use (0, 1, 0) for the up vector

	// Extract the rotated glm::vec3 from the glm::vec4
	glm::vec3 finalRotatedVector = glm::vec3(rotatedVector);

	return finalRotatedVector;
}

glm::vec3 D3D::Camera::GetRight()
{
	// Create the rotation matrix from the quaternion
	glm::mat4 rotationMatrix = glm::mat4_cast(GetRotation());

	// Apply the rotation to the vector using the rotation matrix
	glm::vec4 rotatedVector = rotationMatrix * glm::vec4(1.f, 0.f, 0.f, 0.0f);  // Use (1, 0, 0) for the right vector

	// Extract the rotated glm::vec3 from the glm::vec4
	glm::vec3 finalRotatedVector = glm::vec3(rotatedVector);

	return finalRotatedVector;
}