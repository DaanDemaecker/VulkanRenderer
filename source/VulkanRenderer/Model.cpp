#include "stdafx.h"
#include "Model.h"
#include "VulkanHelpers.h"
#include "VulkanRenderer.h"
#include "Utils.h"

#include <memory>

D3D::Model::Model(bool normal)
{
	if (normal)
	{
		Utils::LoadModel("../Resources/Models/viking_room.obj", m_Vertices, m_Indices);
	}
	else
	{
		Utils::LoadModel("../Resources/Models/vehicle.obj", m_Vertices, m_Indices);
	}
	CreateVertexBuffer();
	CreateIndexBuffer();

	CreateUniformBuffers();

	CreateDescriptorSets();
}

D3D::Model::~Model()
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

void D3D::Model::Render(VkCommandBuffer& commandBuffer, uint32_t frame, const VkDescriptorSet* /*descriptorSet*/)
{
	if (m_UboChanged[frame])
	{
		UpdateUniformBuffer(frame);
	}
	VulkanRenderer::GetInstance().Render(this, commandBuffer, &m_DescriptorSets[frame]);
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

	std::fill(m_UboChanged.begin(), m_UboChanged.end(), true);


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

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = m_UboBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = renderer.GetImageView();
		imageInfo.sampler = renderer.GetSampler();

		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = m_DescriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = m_DescriptorSets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(renderer.GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
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
