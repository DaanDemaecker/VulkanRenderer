#include "DirectionalLightObject.h"

#include "VulkanRenderer3D.h"

D3D::DirectionalLightObject::DirectionalLightObject(VulkanRenderer3D* renderer)
{
	CreateLightBuffer(renderer);
}

D3D::DirectionalLightObject::DirectionalLightObject()
	:DirectionalLightObject(&D3D::VulkanRenderer3D::GetInstance())
{
}

void D3D::DirectionalLightObject::CreateLightBuffer(D3D::VulkanRenderer3D* renderer)
{
	VkDeviceSize bufferSize = sizeof(DirectionalLightStruct);

	auto frames = renderer->GetMaxFrames();

	m_LightBuffers.resize(frames);
	m_LightMemory.resize(frames);
	m_LightMapped.resize(frames);
	m_LightChanged.resize(frames);

	SetDirtyFlags();



	for (size_t i = 0; i < frames; ++i)
	{
		renderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			m_LightBuffers[i], m_LightMemory[i]);

		vkMapMemory(renderer->GetDevice(), m_LightMemory[i], 0, bufferSize, 0, &m_LightMapped[i]);

		UpdateBuffer(static_cast<int>(i));
	}
}

void D3D::DirectionalLightObject::Cleanup(VkDevice device)
{
	for (size_t i{}; i < m_LightBuffers.size(); i++)
	{
		vkDestroyBuffer(device, m_LightBuffers[i], nullptr);
		vkFreeMemory(device, m_LightMemory[i], nullptr);
	}
}

void D3D::DirectionalLightObject::SetDirtyFlags()
{
	std::fill(m_LightChanged.begin(), m_LightChanged.end(), true);
}

void D3D::DirectionalLightObject::UpdateBuffer(int frame)
{
	if (!m_LightChanged[frame])
		return;

	m_LightChanged[frame] = false;

	memcpy(m_LightMapped[frame], &m_BufferObject, sizeof(m_BufferObject));
}

void D3D::DirectionalLightObject::SetDirection(glm::vec3& direction)
{
	m_BufferObject.direction = glm::normalize(direction);
	SetDirtyFlags();
}

void D3D::DirectionalLightObject::SetColor(glm::vec3& color)
{
	m_BufferObject.color = color;
	SetDirtyFlags();
}

void D3D::DirectionalLightObject::SetDirection(glm::vec3&& direction)
{
	m_BufferObject.direction = glm::normalize(direction);
	SetDirtyFlags();
}

void D3D::DirectionalLightObject::SetColor(glm::vec3&& color)
{
	m_BufferObject.color = color;
	SetDirtyFlags();
}

void D3D::DirectionalLightObject::SetIntensity(float intensity)
{
	m_BufferObject.intensity = intensity;
	SetDirtyFlags();
}
