//DirectionalLightObject.cpp

// File includes
#include "DirectionalLightObject.h"
#include "VulkanRenderer3D.h"
#include "Camera.h"

D3D::DirectionalLightObject::DirectionalLightObject()
{
	// Create light buffer
	CreateLightBuffer();
}

void D3D::DirectionalLightObject::CreateLightBuffer()
{
	auto& renderer{ VulkanRenderer3D::GetInstance() };

	// Get the amount of frames in flight
	auto frames = renderer.GetMaxFrames();

	// Resize dirty flags to amount of frames
	m_LightChanged.resize(frames);
	

	// Set all dirty flags to true
	SetDirtyFlags();

	m_DescriptorObject = std::make_unique<UboDescriptorObject<DirectionalLightStruct>>();
}

void D3D::DirectionalLightObject::Cleanup(VkDevice /*device*/)
{
	
}

void D3D::DirectionalLightObject::SetDirtyFlags()
{
	// Fill dirty flag with true values
	std::fill(m_LightChanged.begin(), m_LightChanged.end(), true);
}

void D3D::DirectionalLightObject::UpdateBuffer(int frame)
{
	// Check if dirty flag is set, if not, return
	if (!m_LightChanged[frame])
		return;

	m_DescriptorObject->UpdateUboBuffer(m_BufferObject, frame);

	m_LightChanged[frame] = false;
}

void D3D::DirectionalLightObject::SetDirection(glm::vec3& direction)
{
	// Set new direction after normalizing it
	m_BufferObject.direction = glm::normalize(direction);

	// Set dirty flags
	SetDirtyFlags();
}

void D3D::DirectionalLightObject::SetDirection(glm::vec3&& direction)
{
	// Set new direction after normalizing it
	m_BufferObject.direction = glm::normalize(direction);

	// Set dirty flags
	SetDirtyFlags();
}

void D3D::DirectionalLightObject::SetColor(glm::vec3& color)
{
	// Set new color
	m_BufferObject.color = color;

	// Set dirty flags
	SetDirtyFlags();
}

void D3D::DirectionalLightObject::SetColor(glm::vec3&& color)
{
	// Set new color
	m_BufferObject.color = color;

	// Set dirty flags
	SetDirtyFlags();
}

void D3D::DirectionalLightObject::SetIntensity(float intensity)
{
	// Set new intensity
	m_BufferObject.intensity = intensity;

	// Set dirty flags
	SetDirtyFlags();
}

D3D::DescriptorObject* D3D::DirectionalLightObject::GetDescriptorObject()
{
	return static_cast<DescriptorObject*>(m_DescriptorObject.get());
}

glm::mat4& D3D::DirectionalLightObject::GetLightMatrix()
{
	auto cameraPos = VulkanRenderer3D::GetInstance().GetCamera()->GetPosition();

	// Define a virtual light position far away in the opposite direction of the light relative to the camera position
	glm::vec3 lightPos = cameraPos - m_BufferObject.direction * 1000.0f; // Move the light far away from the camera position

	// Use the camera position as the target
	glm::vec3 target = cameraPos;

	// Define the up vector (can be arbitrary as long as it is not parallel to the light direction)
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	if (glm::dot(up, m_BufferObject.direction) > 0.99f) // If up is almost parallel to the light direction
	{
		up = glm::vec3(1.0f, 0.0f, 0.0f); // Use a different up vector
	}

	glm::mat4 viewMatrix = glm::lookAt(lightPos, target, up);

	glm::mat4 projectionMatrix = glm::ortho(m_OrthoBorders.x, m_OrthoBorders.y, m_OrthoBorders.z, m_OrthoBorders.w, 0.1f, 10000.f);

	// Calculate the view matrix
	m_LightTransform = projectionMatrix * viewMatrix;


	return m_LightTransform;
}
