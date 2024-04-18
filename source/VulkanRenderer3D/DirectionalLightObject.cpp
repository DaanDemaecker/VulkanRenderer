//DirectionalLightObject.cpp

// File includes
#include "DirectionalLightObject.h"
#include "VulkanRenderer3D.h"

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
