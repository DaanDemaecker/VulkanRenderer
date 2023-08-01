#include "stdafx.h"
#include "Model.h"
#include "VulkanRenderer.h"
#include "Utils.h"
#include "Material.h"
#include "TimeManager.h"

#include <memory>

D3D::Model::Model()
{
	m_pMaterial = std::make_shared<D3D::Material>();
}

D3D::Model::~Model()
{
	if (m_Initialized)
	{
		Cleanup();
	}
}

void D3D::Model::LoadModel(const std::string& textPath)
{
	if (m_Initialized)
	{
		m_Initialized = false;
		Cleanup();
	}

	Utils::LoadModel(textPath, m_Vertices, m_Indices);
	CreateVertexBuffer();
	CreateIndexBuffer();
	CreateUniformBuffers();
	CreateDescriptorSets();

	m_Initialized = true;
}

void D3D::Model::SetMaterial(std::shared_ptr<Material> pMaterial)
{
	m_pMaterial = pMaterial;
	if (m_Initialized)
	{
		UpdateDescriptorSets();
	}
}

void D3D::Model::Update()
{
	constexpr float rotSpeed{-glm::radians(15.f)};

	float rotAmount{rotSpeed * TimeManager::GetInstance().GetDeltaTime()};

	SetRotation(m_Rotation.x, m_Rotation.y + rotAmount , m_Rotation.z);
}

void D3D::Model::Render()
{
	if (!m_Initialized)
		return;

	auto& renderer{ VulkanRenderer::GetInstance() };

	auto frame{ renderer.GetCurrentFrame() };

	if (m_UboChanged[frame])
	{
		UpdateUniformBuffer(frame);
	}

	renderer.Render(this, renderer.GetCurrentCommandBuffer(), &m_DescriptorSets[frame], GetPipeline());
}

void D3D::Model::SetPosition(float x, float y, float z)
{
	m_Position = { x, y, z };
	SetDirtyFlags();
}

void D3D::Model::SetRotation(float x, float y, float z)
{
	m_Rotation = { x, y, z };
	SetDirtyFlags();
}

void D3D::Model::SetScale(float x, float y, float z)
{
	m_Scale = { x, y, z };
	SetDirtyFlags();
}

void D3D::Model::CreateVertexBuffer()
{
	auto& renderer{ VulkanRenderer::GetInstance() };
	auto device = renderer.GetDevice();

	VkDeviceSize bufferSize = sizeof(m_Vertices[0]) * m_Vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	renderer.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;

	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, m_Vertices.data(), static_cast<size_t>(bufferSize));
	vkUnmapMemory(device, stagingBufferMemory);

	renderer.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VertexBuffer, m_VertexBufferMemory);

	renderer.CopyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void D3D::Model::CreateIndexBuffer()
{
	auto& renderer{ VulkanRenderer::GetInstance() };
	auto device = renderer.GetDevice();

	VkDeviceSize bufferSize = sizeof(m_Indices[0]) * m_Indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	renderer.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, m_Indices.data(), static_cast<size_t>(bufferSize));

	renderer.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_IndexBuffer, m_IndexBufferMemory);

	renderer.CopyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void D3D::Model::CreateUniformBuffers()
{
	auto& renderer = VulkanRenderer::GetInstance();

	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	m_UboBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	m_UbosMemory.resize(MAX_FRAMES_IN_FLIGHT);
	m_UbosMapped.resize(MAX_FRAMES_IN_FLIGHT);

	m_Ubos.resize(MAX_FRAMES_IN_FLIGHT);
	m_UboChanged.resize(MAX_FRAMES_IN_FLIGHT);

	SetDirtyFlags();


	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		renderer.CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			m_UboBuffers[i], m_UbosMemory[i]);

		vkMapMemory(renderer.GetDevice(), m_UbosMemory[i], 0, bufferSize, 0, &m_UbosMapped[i]);
	}
}

void D3D::Model::CreateDescriptorSets()
{
	auto& renderer{ VulkanRenderer::GetInstance() };

	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, renderer.GetDescriptorSetLayout());
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = renderer.GetDescriptorPool();
	allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = layouts.data();

	m_DescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(renderer.GetDevice(), &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	UpdateDescriptorSets();
}

void D3D::Model::UpdateDescriptorSets()
{
	m_pMaterial->UpdateDescriptorSets(m_UboBuffers, m_DescriptorSets);
}

void D3D::Model::UpdateUniformBuffer(uint32_t frame)
{
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), m_Position);
	glm::vec3 rotationAngles(m_Rotation.x, m_Rotation.y, m_Rotation.z);

	glm::quat quaternion = glm::quat(m_Rotation);
	glm::mat4 rotationMatrix = glm::mat4_cast(quaternion);

	glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), m_Scale);

	m_Ubos[frame].model = translationMatrix * rotationMatrix * scalingMatrix;

	VulkanRenderer::GetInstance().UpdateUniformBuffer(m_Ubos[frame]);

	m_UboChanged[frame] = false;

	memcpy(m_UbosMapped[frame], &m_Ubos[frame], sizeof(m_Ubos[frame]));
}

PipelinePair& D3D::Model::GetPipeline()
{
	if (m_pMaterial != nullptr)
	{
		return m_pMaterial->GetPipeline();
	}

	return VulkanRenderer::GetInstance().GetPipeline();
}

void D3D::Model::Cleanup()
{
	auto device = D3D::VulkanRenderer::GetInstance().GetDevice();

	vkDeviceWaitIdle(device);

	vkDestroyBuffer(device, m_IndexBuffer, nullptr);
	vkFreeMemory(device, m_IndexBufferMemory, nullptr);

	vkDestroyBuffer(device, m_VertexBuffer, nullptr);
	vkFreeMemory(device, m_VertexBufferMemory, nullptr);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		vkDestroyBuffer(device, m_UboBuffers[i], nullptr);
		vkFreeMemory(device, m_UbosMemory[i], nullptr);
	}
}

void D3D::Model::SetDirtyFlags()
{
	std::fill(m_UboChanged.begin(), m_UboChanged.end(), true);
}
