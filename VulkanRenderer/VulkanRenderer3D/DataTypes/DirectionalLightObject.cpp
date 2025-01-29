//DirectionalLightObject.cpp

// Header include
#include "DirectionalLightObject.h"

// File includes
#include "Vulkan/Vulkan3D.h"

#include "Camera.h"

#include "Utils/Utils.h"

// Standard library includes
#include <iostream>

DDM3::DirectionalLightObject::DirectionalLightObject()
{
	// Create light buffer
	CreateLightBuffer();
}

DDM3::DirectionalLightObject::~DirectionalLightObject()
{
	Cleanup(Vulkan3D::GetInstance().GetDevice());
}

void DDM3::DirectionalLightObject::CreateLightBuffer()
{
	// Get the amount of frames in flight
	int frames = static_cast<int>(Vulkan3D::GetMaxFrames());

	// Resize dirty flags to amount of frames
	m_LightChanged.resize(frames);
	

	// Set all dirty flags to true
	SetDirtyFlags();

	m_DescriptorObject = std::make_unique<UboDescriptorObject<DirectionalLightStruct>>();

	m_LightMatrixDescriptorObject = std::make_unique<UboDescriptorObject<glm::mat4>>();

	for (int i{}; i < frames; i++)
	{
		CalculateLightTransform(i);
	}
}

void DDM3::DirectionalLightObject::Cleanup(VkDevice /*device*/)
{
	
}

void DDM3::DirectionalLightObject::SetDirtyFlags()
{
	// Fill dirty flag with true values
	std::fill(m_LightChanged.begin(), m_LightChanged.end(), true);
}

void DDM3::DirectionalLightObject::CalculateLightTransform(int frame)
{
	auto cameraPos = Vulkan3D::GetInstance().GetCurrentCamera()->GetPosition();
	//cameraPos = glm::vec3{};

	// Define a virtual light position far away in the opposite direction of the light relative to the camera position
	glm::vec3 lightPos = cameraPos - m_BufferObject.direction * (m_ClippingDistance * 0.5f); // Move the light far away from the camera position

	// Create rotation matrix
	glm::mat4 rotationMatrix = glm::mat4_cast(glm::conjugate(Utils::RotationFromDirection(m_BufferObject.direction)));

	// Create translation matrix
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), -lightPos);

	// Multiply matrices (apply rotation first, then translation)
	glm::mat4 viewMatrix = rotationMatrix * translationMatrix;

	// Create orthographic projection matrix
	glm::mat4 projectionMatrix = glm::ortho(m_OrthoBorders.x, m_OrthoBorders.y, m_OrthoBorders.z, m_OrthoBorders.w, 0.1f, m_ClippingDistance);

	// Adjust the projection matrix for Vulkan
	// Negate the Y-axis to flip it for Vulkan's coordinate system
	projectionMatrix[1][1] *= -1;
	projectionMatrix[2][2] *= -1;
	projectionMatrix[2][3] *= -1;

	// Calculate the light transform matrix
	m_LightTransform = projectionMatrix * viewMatrix;

	// Update the UBO with the new light transform matrix
	m_LightMatrixDescriptorObject->UpdateUboBuffer(m_LightTransform, frame);


	//VulkanRenderer3D::GetInstance().GetCamera()->SetPosition(lightPos);
	//VulkanRenderer3D::GetInstance().GetCamera()->SetDirection(m_BufferObject.direction);
}

void DDM3::DirectionalLightObject::UpdateBuffer(int frame)
{
	CalculateLightTransform(frame);
	// Check if dirty flag is set, if not, return
	if (!m_LightChanged[frame])
		return;

	m_DescriptorObject->UpdateUboBuffer(m_BufferObject, frame);

	m_LightChanged[frame] = false;
}

void DDM3::DirectionalLightObject::SetDirection(glm::vec3& direction)
{
	// Set new direction after normalizing it
	m_BufferObject.direction = glm::normalize(direction);

	// Set dirty flags
	SetDirtyFlags();
}

void DDM3::DirectionalLightObject::SetDirection(glm::vec3&& direction)
{
	// Set new direction after normalizing it
	m_BufferObject.direction = glm::normalize(direction);

	// Set dirty flags
	SetDirtyFlags();
}

void DDM3::DirectionalLightObject::SetColor(glm::vec3& color)
{
	// Set new color
	m_BufferObject.color = color;

	// Set dirty flags
	SetDirtyFlags();
}

void DDM3::DirectionalLightObject::SetColor(glm::vec3&& color)
{
	// Set new color
	m_BufferObject.color = color;

	// Set dirty flags
	SetDirtyFlags();
}

void DDM3::DirectionalLightObject::SetIntensity(float intensity)
{
	// Set new intensity
	m_BufferObject.intensity = intensity;

	// Set dirty flags
	SetDirtyFlags();
}

DDM3::DescriptorObject* DDM3::DirectionalLightObject::GetDescriptorObject()
{
	return static_cast<DescriptorObject*>(m_DescriptorObject.get());
}

DDM3::DescriptorObject* DDM3::DirectionalLightObject::GetTransformDescriptorObject()
{
	return static_cast<DescriptorObject*>(m_LightMatrixDescriptorObject.get());
}

glm::mat4& DDM3::DirectionalLightObject::GetLightMatrix()
{
	return m_LightTransform;
}