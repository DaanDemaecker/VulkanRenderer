// TexturedMaterial.cpp

// File includes
#include "TexturedMaterial.h"
#include "VulkanRenderer3D.h"
#include "Utils.h"
#include "DescriptorPoolManager.h"
#include "DescriptorPoolWrapper.h"
#include "STBIncludes.h"

D3D::TexturedMaterial::TexturedMaterial(std::initializer_list<const std::string> filePaths, const std::string& pipelineName)
	:Material(pipelineName)
{
	// Set texture amount to the size of the initializer list
	m_TextureAmount = static_cast<int>(filePaths.size());
	
	// Resize textureImages to textureAmount
	m_TextureImages.resize(m_TextureAmount);
	// Resize textureImageViews to textureAmount
	m_TextureImageViews.resize(m_TextureAmount);
	// Resize textureImageMemories to textureAmount
	m_TextureImageMemories.resize(m_TextureAmount);

	// Initialize index variable
	int index{};

	// Loop trough all filePaths
	for (const auto& path : filePaths)
	{
		// Create texture image with current path and index
		CreateTextureImage(path, index);
		// Create texture image view with current index
		CreateTextureImageView(index);
		// Increment index
		++index;
	}

	// Create sampler
	CreateTextureSampler();
}

D3D::TexturedMaterial::~TexturedMaterial()
{
	// Get reference to device for later use
	auto& device{ VulkanRenderer3D::GetInstance().GetDevice() };

	// Loop trough imageviews and destroy them
	for (auto& imageView : m_TextureImageViews)
	{
		vkDestroyImageView(device, imageView, nullptr);
	}

	// Loop trough textureImages and destroy them
	for (auto& texture : m_TextureImages)
	{
		vkDestroyImage(device, texture, nullptr);
	}

	// Loop trouh textureImageMemories and free them
	for (auto& memory : m_TextureImageMemories)
	{
		vkFreeMemory(device, memory, nullptr);
	}
}

void D3D::TexturedMaterial::CreateDescriptorSets(Model* pModel, std::vector<VkDescriptorSet>& descriptorSets)
{
	// Get descriptorpool associated with this material
	auto descriptorPool = GetDescriptorPool();
	// Add model to descriptorpool
	descriptorPool->AddModel(pModel);
	// Create descriptorpool
	descriptorPool->CreateDescriptorSets(GetDescriptorLayout(), descriptorSets);
}

void D3D::TexturedMaterial::UpdateDescriptorSets(std::vector<VkBuffer>& uboBuffers, std::vector<VkDescriptorSet>& descriptorSets)
{
	// Get descriptorpool associated with this material
	auto descriptorPool = GetDescriptorPool();
	// Create vector of vector of ubo buffers
	std::vector<std::vector<VkBuffer>> uboBuffferList{ uboBuffers, D3D::VulkanRenderer3D::GetInstance().GetLightBuffers() };

	// Create vector for buffersizes
	std::vector<VkDeviceSize> uboSizes(2);
	// Set first size to size of UniformBufferObject
	uboSizes[0] = sizeof(UniformBufferObject);
	// Set second size to size of LightObject
	uboSizes[1] = sizeof(DirectionalLightObject);

	// Update descriptorsets
	descriptorPool->UpdateDescriptorSets(descriptorSets, uboBuffferList, uboSizes,  m_TextureImageViews.data());
}

std::vector<VkDescriptorSetLayout>& D3D::TexturedMaterial::GetDescriptorLayout()
{
	// Get descriptorlayout for this material
	// Textured material standardly has 1 veretx ubo, 1 fragment ubo and the amount of textures that was requested
	return VulkanRenderer3D::GetInstance().GetDescriptorSetLayout(1, 1, static_cast<int>(m_TextureImages.size()));
}

D3D::DescriptorPoolWrapper* D3D::TexturedMaterial::GetDescriptorPool()
{
	// Get descriptorpool for this material
	// Textured material standardly has 2 ubos and the amount of textures that was requested
	return D3D::VulkanRenderer3D::GetInstance().GetDescriptorPoolManager()->GetDescriptorPool(2, m_TextureAmount);
}

void D3D::TexturedMaterial::CreateTextureImage(const std::string& filePath, int index)
{
	// Get reference to renderer for later use
	auto& renderer{ VulkanRenderer3D::GetInstance() };
	// Get reference to logical device for later use
	auto& device{ renderer.GetDevice() };

	// Create variable for width, height and channels for textures
	int texWidth, texHeight, texChannels;

	// Load image into pixels
	stbi_uc* pixels = stbi_load(filePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	// Calculate the amount of levels the mipmap will have
	m_MipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

	// Calculate image size
	VkDeviceSize imageSize = static_cast<uint64_t>(texWidth) * static_cast<uint64_t>(texHeight) * static_cast<uint64_t>(4);

	// Check if pixels were loaded in correctly, if not, throw runtime error
	if (!pixels)
	{
		throw std::runtime_error("failed to load texture image!");
	}

	// Initialize staging buffer
	VkBuffer stagingBuffer{};
	// Initialize staging buffer memory
	VkDeviceMemory stagingBufferMemory{};

	// Create staging buffer
	renderer.CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingBufferMemory);
	
	// Create void pointer
	void* data;

	// Map memory date to staginbuffermemory
	vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
	// Copy memory of pixels to data pointer
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	// Unmap memory fo stagingbuffermemory
	vkUnmapMemory(device, stagingBufferMemory);

	// Free pixels
	stbi_image_free(pixels);

	// Create image
	renderer.CreateImage(texWidth, texHeight, m_MipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		m_TextureImages[index], m_TextureImageMemories[index]);

	// Transition image layout
	renderer.TransitionImageLayout(m_TextureImages[index], VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_MipLevels);
	// Copy staginbuffer to textureImage
	renderer.CopyBufferToImage(stagingBuffer, m_TextureImages[index], static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

	// Destroy staging buffer
	vkDestroyBuffer(device, stagingBuffer, nullptr);
	// Free staging buffer memory
	vkFreeMemory(device, stagingBufferMemory, nullptr);

	// Generate mipmaps
	renderer.GenerateMipmaps(m_TextureImages[index], VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, m_MipLevels);
}

void D3D::TexturedMaterial::CreateTextureImageView(int index)
{
	// Create texture image view
	m_TextureImageViews[index] = D3D::VulkanRenderer3D::GetInstance().CreateImageView(m_TextureImages[index], VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, m_MipLevels);
}

void D3D::TexturedMaterial::CreateTextureSampler()
{
	// Get sampler
	m_TextureSampler = VulkanRenderer3D::GetInstance().GetSampler();
}
