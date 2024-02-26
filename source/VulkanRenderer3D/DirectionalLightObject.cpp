//DirectionalLightObject.cpp

// File includes
#include "DirectionalLightObject.h"
#include "VulkanRenderer3D.h"

D3D::DirectionalLightObject::DirectionalLightObject(VulkanRenderer3D* renderer)
{
	// Create light buffer
	CreateLightBuffer(renderer);
}

D3D::DirectionalLightObject::DirectionalLightObject()
	// Call constructor after getting renderer
	:DirectionalLightObject(&D3D::VulkanRenderer3D::GetInstance())
{
}

void D3D::DirectionalLightObject::CreateLightBuffer(D3D::VulkanRenderer3D* renderer)
{
	// Get size of the struct for the size of the buffer
	VkDeviceSize bufferSize = sizeof(DirectionalLightStruct);
	
	// Get the amount of frames in flight
	auto frames = renderer->GetMaxFrames();
	
	// Resize lightbuffers to amount of frames
	m_LightBuffers.resize(frames);

	// Resize memories to amount of frames
	m_LightMemory.resize(frames);

	// Resize mapped memories to amount of frames
	m_LightMapped.resize(frames);

	// Resize dirty flags to amount of frames
	m_LightChanged.resize(frames);
	

	// Set all dirty flags to true
	SetDirtyFlags();

	// Loop trough the amount of frames
	for (size_t i = 0; i < frames; ++i)
	{
		// Create the buffer for the light
		renderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			m_LightBuffers[i], m_LightMemory[i]);

		// Map the memory for the buffers
		vkMapMemory(renderer->GetDevice(), m_LightMemory[i], 0, bufferSize, 0, &m_LightMapped[i]);

		// Update the current buffer
		UpdateBuffer(static_cast<int>(i));
	}
}

void D3D::DirectionalLightObject::Cleanup(VkDevice device)
{
	// Loop trough the amount of frames
	for (size_t i{}; i < m_LightBuffers.size(); i++)
	{
		// Destroy buffer
		vkDestroyBuffer(device, m_LightBuffers[i], nullptr);
		
		// Free memory
		vkFreeMemory(device, m_LightMemory[i], nullptr);
	}
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

	// Reset dirty flag
	m_LightChanged[frame] = false;

	// Copy memory of bufferobject to mapped memory
	memcpy(m_LightMapped[frame], &m_BufferObject, sizeof(m_BufferObject));
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
