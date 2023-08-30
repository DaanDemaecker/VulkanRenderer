#include "stdafx.h"
#include "TexturedMaterial.h"
#include "VulkanRenderer.h"
#include "Utils.h"

#include <stb_image.h>

D3D::TexturedMaterial::TexturedMaterial(std::initializer_list<const std::string> filePaths, const std::string& pipelineName)
	:Material(pipelineName)
{
	m_TextureImages.resize(filePaths.size());
	m_TextureImageViews.resize(filePaths.size());
	m_TextureImageMemories.resize(filePaths.size());

	int index{};

	for (const auto& path : filePaths)
	{
		CreateTextureImage(path, index);
		CreateTextureImageView(index);
		++index;
	}
	CreateTextureSampler();
}

D3D::TexturedMaterial::~TexturedMaterial()
{
	auto& device{ VulkanRenderer::GetInstance().GetDevice() };

	for (auto& imageView : m_TextureImageViews)
	{
		vkDestroyImageView(device, imageView, nullptr);
	}

	for (auto& texture : m_TextureImages)
	{
		vkDestroyImage(device, texture, nullptr);
	}

	for (auto& memory : m_TextureImageMemories)
	{
		vkFreeMemory(device, memory, nullptr);
	}
}

void D3D::TexturedMaterial::UpdateDescriptorSets(std::vector<VkBuffer>& uboBuffers, std::vector<VkDescriptorSet>& descriptorSets)
{
	auto& renderer{ D3D::VulkanRenderer::GetInstance() };

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		std::vector<VkWriteDescriptorSet> descriptorWrites{};
		descriptorWrites.resize(m_TextureImageViews.size() + 1);

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uboBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		std::vector<VkDescriptorImageInfo> imageInfos{};
		imageInfos.reserve(m_TextureImageViews.size());

		for (auto& imageView : m_TextureImageViews)
		{
			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = imageView;
			imageInfo.sampler = renderer.GetSampler();

			imageInfos.push_back(imageInfo);
		}

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = descriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		for (int j{1}; j <= m_TextureImageViews.size(); ++j)
		{
			descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[j].dstSet = descriptorSets[i];
			descriptorWrites[j].dstBinding = j;
			descriptorWrites[j].dstArrayElement = 0;
			descriptorWrites[j].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[j].descriptorCount = 1;
			descriptorWrites[j].pImageInfo = &imageInfos[j-1];
		}

		vkUpdateDescriptorSets(VulkanRenderer::GetInstance().GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

VkDescriptorSetLayout* D3D::TexturedMaterial::GetDescriptorLayout()
{
	return VulkanRenderer::GetInstance().GetDescriptorSetLayout(static_cast<int>(m_TextureImages.size()));
}

VkDescriptorPool& D3D::TexturedMaterial::GetDescriptorPool()
{
	return D3D::VulkanRenderer::GetInstance().GetDescriptorPool();
}

void D3D::TexturedMaterial::CreateTextureImage(const std::string& filePath, int index)
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
		m_TextureImages[index], m_TextureImageMemories[index]);

	renderer.TransitionImageLayout(m_TextureImages[index], VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_MipLevels);
	renderer.CopyBufferToImage(stagingBuffer, m_TextureImages[index], static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

	///transitionImageLayout(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_MipLevels);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);

	renderer.GenerateMipmaps(m_TextureImages[index], VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, m_MipLevels);
}

void D3D::TexturedMaterial::CreateTextureImageView(int index)
{
	m_TextureImageViews[index] = D3D::VulkanRenderer::GetInstance().CreateImageView(m_TextureImages[index], VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, m_MipLevels);
}

void D3D::TexturedMaterial::CreateTextureSampler()
{
	m_TextureSampler = VulkanRenderer::GetInstance().GetSampler();
}
