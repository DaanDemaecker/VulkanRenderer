#include "TexturedMaterial.h"
#include "VulkanRenderer.h"
#include "Utils.h"
#include "DescriptorPoolManager.h"
#include "DescriptorPoolWrapper.h"

#include "STBIncludes.h"

D3D::TexturedMaterial::TexturedMaterial(std::initializer_list<const std::string> filePaths, const std::string& pipelineName)
	:Material(pipelineName)
{
	m_TextureAmount = static_cast<int>(filePaths.size());

	m_TextureImages.resize(m_TextureAmount);
	m_TextureImageViews.resize(m_TextureAmount);
	m_TextureImageMemories.resize(m_TextureAmount);

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

void D3D::TexturedMaterial::CreateDescriptorSets(Model* pModel, std::vector<VkDescriptorSet>& descriptorSets)
{
	auto descriptorPool = GetDescriptorPool();
	descriptorPool->AddModel(pModel);
	descriptorPool->CreateDescriptorSets(GetDescriptorLayout(), descriptorSets);
}

void D3D::TexturedMaterial::UpdateDescriptorSets(std::vector<VkBuffer>& uboBuffers, std::vector<VkDescriptorSet>& descriptorSets)
{
	auto descriptorPool = GetDescriptorPool();
	std::vector<std::vector<VkBuffer>> uboBuffferList{ uboBuffers, D3D::VulkanRenderer::GetInstance().GetLightBuffers() };

	std::vector<VkDeviceSize> uboSizes(2);
	uboSizes[0] = sizeof(UniformBufferObject);
	uboSizes[1] = sizeof(LightObject);

	descriptorPool->UpdateDescriptorSets(descriptorSets, uboBuffferList, uboSizes,  m_TextureImageViews.data());
}

std::vector<VkDescriptorSetLayout>& D3D::TexturedMaterial::GetDescriptorLayout()
{
	return VulkanRenderer::GetInstance().GetDescriptorSetLayout(1, 1, static_cast<int>(m_TextureImages.size()));
}

D3D::DescriptorPoolWrapper* D3D::TexturedMaterial::GetDescriptorPool()
{
	return D3D::VulkanRenderer::GetInstance().GetDescriptorPoolManager()->GetDescriptorPool(2, m_TextureAmount);
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
