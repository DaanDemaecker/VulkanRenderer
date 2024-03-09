#include "Camera.h"


#include "TimeManager.h"


void D3D::Camera::UpdateUniformBuffer(UniformBufferObject& buffer)
{
	// If the camera transform has changed, update matrix
	if (m_HasChanged)
		UpdateMatrix();

	// Set buffer view matrix
	buffer.view = m_Matrix;
}

void D3D::Camera::UpdateMatrix()
{
	// Set hasChanged to false
	m_HasChanged = false;

	// Create translation matrix
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), m_Position);

	// Cast rotation vector to quaternion
	glm::quat quaternion = glm::quat(m_Rotation);
	// Create rotation matrix
	glm::mat4 rotationMatrix = glm::mat4_cast(quaternion);
	// Create scaling matrix
	glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), m_Scale);

	// Multiply matrices
	m_Matrix = translationMatrix * rotationMatrix * scalingMatrix;
}
