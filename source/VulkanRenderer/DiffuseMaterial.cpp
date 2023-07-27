#include "stdafx.h"
#include "DiffuseMaterial.h"
#include "VulkanRenderer.h"
#include "Utils.h"

#include <stb_image.h>

D3D::DiffuseMaterial::DiffuseMaterial(const std::string& filePath, const std::string& pipelineName)
	:Material(pipelineName)
{
	CreateTextureImage(filePath);
	CreateTextureImageView();
	CreateTextureSampler();
}

D3D::DiffuseMaterial::~DiffuseMaterial()
{
	auto& device{ VulkanRenderer::GetInstance().GetDevice() };

	vkDestroyImageView(device, m_TextureImageView, nullptr);
	vkDestroyImage(device, m_TextureImage, nullptr);
	vkFreeMemory(device, m_TextureImageMemory, nullptr);
}

void D3D::DiffuseMaterial::UpdateDescriptorSets(std::vector<VkBuffer>& uboBuffers, std::vector<VkDescriptorSet>& descriptorSets)
{
	auto& renderer{ D3D::VulkanRenderer::GetInstance() };

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uboBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = m_TextureImageView;
		imageInfo.sampler = renderer.GetSampler();

		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = descriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = descriptorSets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(VulkanRenderer::GetInstance().GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

void D3D::DiffuseMaterial::CreateTextureImage(const std::string& filePath)
{
	auto& renderer{ VulkanRenderer::GetInstance() };
	auto& device{ renderer.GetDevice() };

	int texWidth, texHeight, texChannels;

	stbi_uc* pixels = stbi_load(filePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	m_MipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

	VkDeviceSize imageSize = static_cast<uint64_t>(texWidth) * static_cast<uint64_t>(texHeight) * static_cast<uint64_t>(4);

	if (!pixels)
	{
		throw std::runtime_error("failed to load texture image!");
	}

	VkBuffer stagingBuffer{};
	VkDeviceMemory stagingBufferMemory{};

	renderer.CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingBufferMemory);

	void* data;

	vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(device, stagingBufferMemory);

	stbi_image_free(pixels);

	renderer.CreateImage(texWidth, texHeight, m_MipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		m_TextureImage, m_TextureImageMemory);

	renderer.TransitionImageLayout(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_MipLevels);
	renderer.CopyBufferToImage(stagingBuffer, m_TextureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

	///transitionImageLayout(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_MipLevels);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);

	renderer.GenerateMipmaps(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, m_MipLevels);
}

void D3D::DiffuseMaterial::CreateTextureImageView()
{
	m_TextureImageView = D3D::VulkanRenderer::GetInstance().CreateImageView(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, m_MipLevels);
}

void D3D::DiffuseMaterial::CreateTextureSampler()
{
	m_TextureSampler = VulkanRenderer::GetInstance().GetSampler();
}
