// ImageManager.cpp

// File includes
#include "ImageManager.h"
#include "STBIncludes.h"
#include "VulkanUtils.h"
#include "GPUObject.h"
#include "BufferManager.h"
#include "ConfigManager.h"

// Standard library includes
#include <stdexcept>
#include <cmath>

D3D::ImageManager::ImageManager(GPUObject* pGPUObject, D3D::BufferManager* pBufferManager, CommandpoolManager* pCommandPoolManager)
	:m_DefaultTextureName{ConfigManager::GetInstance().GetString("DefaultTextureName")}
{
	// Initialize the default textures
	CreateDefaultResources(pGPUObject, pBufferManager, pCommandPoolManager);
}

void D3D::ImageManager::CreateDefaultResources(GPUObject* pGPUObject, D3D::BufferManager* pBufferManager,CommandpoolManager* pCommandPoolManager)
{
	// Create the default texture sampler
	CreateTextureSampler(pGPUObject, m_TextureSampler, m_DefaultTexture.mipLevels);
	// Create the default texture image
	CreateTextureImage(pGPUObject, pBufferManager, m_DefaultTexture, m_DefaultTextureName, pCommandPoolManager);
	// Create the default texture image view
	m_DefaultTexture.imageView = CreateImageView(pGPUObject->GetDevice(), m_DefaultTexture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, m_DefaultTexture.mipLevels);
}

VkImageView D3D::ImageManager::CreateImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
{
	// Create image view create info
	VkImageViewCreateInfo viewInfo{};
	// Set type to image view create info
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	// Give handle of image
	viewInfo.image = image;
	// Set view type to 2D
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	// Give handle of requested format
	viewInfo.format = format;
	// Give aspectflags
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	// Set base mip level to 0
	viewInfo.subresourceRange.baseMipLevel = 0;
	// Set levelcount to the amount of miplevels
	viewInfo.subresourceRange.levelCount = mipLevels;
	// Set base array layer to 0
	viewInfo.subresourceRange.baseArrayLayer = 0;
	// Set layercount to 1
	viewInfo.subresourceRange.layerCount = 1;

	// Create image view handle
	VkImageView imageView;
	// Create image view
	if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to create texture image view!");
	}

	// Return image view handle
	return imageView;
}

void D3D::ImageManager::CreateCubeTexture(GPUObject* pGPUObject, D3D::BufferManager* pBufferManager, Texture& cubeTexture, const std::initializer_list<const std::string>& textureNames, CommandpoolManager* pCommandPoolManager)
{
	// Get device
	auto device{ pGPUObject->GetDevice() };

	// Get the amount of images from the length of the textureNames list
	uint32_t imageCount{ static_cast<uint32_t>(textureNames.size()) };

	// If there are less than 6 images, no cube map can be made
	if (imageCount < 6)
	{
		// Throw runtime error
		throw std::runtime_error("6 or more images are required for a cube map");
	}

	// Initialize texture width, height and channels
	int texWidth{};
	int texHeight{};
	int texChannels{};

	// Load in the first image of the list to get the width, height and channel amount
	stbi_uc* pixels = stbi_load(textureNames.begin()->c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	// If pixels failed to load in, throw runtime error
	if (!pixels)
	{
		throw std::runtime_error("Failed to load texture image!");
	}

	// Set miplevels to 1 as cube textures generally don't need them
	cubeTexture.mipLevels = 1;
	// Calculate the size of a single image
	VkDeviceSize faceSize = texWidth * texHeight * texChannels;
	// Calcualte the size of the entire cubemap
	VkDeviceSize cubeSize = faceSize * imageCount;

	// Create staging buffer object
	VkBuffer stagingBuffer{};
	// Create staging buffer memory object
	VkDeviceMemory stagingBufferMemory{};

	// Create the staging buffer, make it the size of the entire cube
		pBufferManager->CreateBuffer(pGPUObject, cubeSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingBufferMemory);

	// Create void pointer to hold data
	void* data;

	// Map the memory of the staging buffer memory to the data pointer
	vkMapMemory(device, stagingBufferMemory, 0, cubeSize, 0, &data);
	// Copy the data from the pixels to the data pointer
	memcpy(data, pixels, static_cast<size_t>(faceSize));

	// Free the pixels
	stbi_image_free(pixels);

	// Loop trough all the other images and load them in
	for (uint32_t i = 1; i < imageCount; i++)
	{
		// Load in the pixels
		pixels = stbi_load((textureNames.begin() + i)->c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

		// If pixels failed to load in, throw runtime error
		if (!pixels)
		{
			throw std::runtime_error("Failed to load texture image!");
		}

		// Cast the void ptr to a char*
		char* mem_offset{ reinterpret_cast<char*>(data) };

		// Calculate the offset of the current image and add it to the pointer
		mem_offset += i * faceSize;

		// Copy the memory of the pixels with the correct offset
		memcpy(mem_offset, pixels, static_cast<size_t>(faceSize));


		// Free the pixels
		stbi_image_free(pixels);
	}

	// Create image create info
	VkImageCreateInfo imageCreateInfo = {};
	// Set type to image create info
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	// Set type to image type 2D, even for cube map
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	// Set the format to R8G8B8A8
	imageCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
	// Set the width and height of the texture as the extent
	imageCreateInfo.extent = { static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1 };
	// Give the miplevels
	imageCreateInfo.mipLevels = cubeTexture.mipLevels;
	// Set arrayLayers to the amount of textures
	imageCreateInfo.arrayLayers = imageCount;
	// Set samples to 1 bit
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	// Set tiling to optimal
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	// Set usage to transfer destination and sampling
	imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	// Set sharing mode to exclusive
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	// Set initial layout to undefined
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	// Set flags to cube compatible
	imageCreateInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

	// Create the image
	if (vkCreateImage(device, &imageCreateInfo, nullptr, &cubeTexture.image) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create cube map image!");
	}

	// Create memory requirements object
	VkMemoryRequirements memoryRequirements;
	// Get the memory requirements
	vkGetImageMemoryRequirements(device, cubeTexture.image, &memoryRequirements);

	// Create allocate info
	VkMemoryAllocateInfo allocateInfo = {};
	// Set type to memory allocate info
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	// Set allocation size to the size found in memory requirements
	allocateInfo.allocationSize = memoryRequirements.size;
	// Find the correct memory type
	allocateInfo.memoryTypeIndex = VulkanUtils::FindMemoryType(pGPUObject->GetPhysicalDevice(), memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	// Allocate the memory
	if (vkAllocateMemory(device, &allocateInfo, nullptr, &cubeTexture.imageMemory) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("Failed to allocate memory for cube map image!");
	}

	// Bind the memory
	vkBindImageMemory(device, cubeTexture.image, cubeTexture.imageMemory, 0);


	// Create a command buffer object
	VkCommandBuffer commandBuffer{};

	// Get single time command buffer
	commandBuffer = pCommandPoolManager->BeginSingleTimeCommands(device);
	// Transition the image layout from undifined to transfer destination optimal
	TransitionImageLayout(cubeTexture.image, commandBuffer, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, cubeTexture.mipLevels, imageCount);
	// End single time command buffer
	pCommandPoolManager->EndSingleTimeCommands(pGPUObject, commandBuffer);

	// Get new single time command buffer
	commandBuffer = pCommandPoolManager->BeginSingleTimeCommands(device);
	// Coppy staging buffer to texture image
	CopyBufferToImage(commandBuffer, stagingBuffer, cubeTexture.image,
		static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), imageCount);
	// End single time command buffer
	pCommandPoolManager->EndSingleTimeCommands(pGPUObject, commandBuffer);

	// Get single time command buffer
	commandBuffer = pCommandPoolManager->BeginSingleTimeCommands(device);
	// Transition the image layout from undifined to transfer destination optimal
	TransitionImageLayout(cubeTexture.image, commandBuffer, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, cubeTexture.mipLevels, imageCount);
	// End single time command buffer
	pCommandPoolManager->EndSingleTimeCommands(pGPUObject, commandBuffer);

	// Destroy the staging buffer
	vkDestroyBuffer(device, stagingBuffer, nullptr);
	// Free the staging buffer memory
	vkFreeMemory(device, stagingBufferMemory, nullptr);


	// Create image view create info
	VkImageViewCreateInfo viewInfo{};
	// Set type to image view create info
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	// Give handle of image
	viewInfo.image = cubeTexture.image;
	// Set view type to 2D
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
	// Give handle of requested format
	viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
	// Give aspectflags
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	// Set base mip level to 0
	viewInfo.subresourceRange.baseMipLevel = 0;
	// Set levelcount to the amount of miplevels
	viewInfo.subresourceRange.levelCount = cubeTexture.mipLevels;
	// Set base array layer to 0
	viewInfo.subresourceRange.baseArrayLayer = 0;
	// Set layercount to 1
	viewInfo.subresourceRange.layerCount = imageCount;

	// Create image view
	if (vkCreateImageView(device, &viewInfo, nullptr, &cubeTexture.imageView) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to create texture image view!");
	}
}

void D3D::ImageManager::CreateTextureSampler(GPUObject* pGPUObject, VkSampler& sampler, uint32_t miplevels)
{
	// Create sampler create info
	VkSamplerCreateInfo samplerInfo{};
	// Set type to sampler create info
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	// Set magfilter to linear
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	// Set minfilter to linear
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	// Set adressmode for U coordinates to repeat
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	// Set adressmode for V coordinates to repeat
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	// Set adressmode for W coordinates to repeat
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	// Create handle of physical device properties
	VkPhysicalDeviceProperties properties{};
	// Get the physical device properties
	vkGetPhysicalDeviceProperties(pGPUObject->GetPhysicalDevice(), &properties);

	// Set anisotropy to enabled
	samplerInfo.anisotropyEnable = VK_TRUE;
	// Set max anisotropy to the limit of the physical device properties
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	// Set the border color to black
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	// Disable coordinate nomalization
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	// Disable compare function
	samplerInfo.compareEnable = VK_FALSE;
	// Set compare function to compare always just in case
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	// Set mipmap mode to linear
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	// Set miplodbias to 0
	samplerInfo.mipLodBias = 0.0f;
	// Set minlod to 0
	samplerInfo.minLod = 0.0f;
	// Set maxlod to amount of miplevels
	samplerInfo.maxLod = static_cast<float>(miplevels);

	// Create the image sampler
	if (vkCreateSampler(pGPUObject->GetDevice(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to create texture sampler!");
	}
}

void D3D::ImageManager::Cleanup(VkDevice device)
{
	// Destroy the sampler
	vkDestroySampler(device, m_TextureSampler, nullptr);
	// Call cleanup function for texture
	m_DefaultTexture.Cleanup(device);
}

void D3D::ImageManager::CopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount)
{
	// Create buffer image copy
	VkBufferImageCopy region{};
	// Set offset to 0
	region.bufferOffset = 0;
	// Set rowlength to 0
	region.bufferRowLength = 0;
	// Set image height to 0
	region.bufferImageHeight = 0;

	// Set aspectmask of subresource to color
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	// Set miplevel of subresource to miplevel
	region.imageSubresource.mipLevel = 0;
	// Set base array layer of subresource to 0
	region.imageSubresource.baseArrayLayer = 0;
	// Set layercount of subresource to layerCount
	region.imageSubresource.layerCount = layerCount;
	
	// Set image offset to 0, 0, 0
	region.imageOffset = { 0, 0, 0 };
	// Set image extent to the width and the height of the image and 1 for the z component
	region.imageExtent =
	{
		width,
		height,
		1
	};

	// Copy the buffer to the image
	vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

void D3D::ImageManager::GenerateMipmaps(VkPhysicalDevice physicalDevice, VkCommandBuffer commandBuffer, VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
{
	// Create format properties object
	VkFormatProperties formatProperties{};
	// Get format properties
	vkGetPhysicalDeviceFormatProperties(physicalDevice, imageFormat, &formatProperties);

	// Check if the format properties have optimal tiling features
	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
	{
		// If not, throw runtime error
		throw std::runtime_error("texture image format does not support linear blitting!");
	}

	// Create memory barrier object
	VkImageMemoryBarrier barrier{};
	// Set type to image memory barrier
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	// Set image to the requested image
	barrier.image = image;
	// Set source queuefamily index to ignored
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	// Set destination family index to ignored
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	// Set aspectmask of subresource to color
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	// Set base array layer of sub resource to 0
	barrier.subresourceRange.baseArrayLayer = 0;
	// Set layer count of subresource to 1
	barrier.subresourceRange.layerCount = 1;
	// Set level count of subresource to 1
	barrier.subresourceRange.levelCount = 1;

	// Initialize mipwidth with the width of the texture
	int32_t mipWidth = texWidth;
	// Initialize mipheight with the height of the texture
	int32_t mipHeight = texHeight;

	// Loop trough the amount of miplevels
	for (uint32_t i = 1; i < mipLevels; i++)
	{
		// Set base miplevel to i -1
		barrier.subresourceRange.baseMipLevel = i - 1;
		// Set old layout to transfer destination optimal
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		// Set new layout to transfer source optimal
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		// Set source access mask to transfer write bit
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		// Set destination access mask to transfer read bit
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		// Create barrier
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
			0, nullptr, 0, nullptr, 1, &barrier);

		// Create image blit object
		VkImageBlit blit{};
		// Set source offsets of offset 0 to 0, 0, 0
		blit.srcOffsets[0] = { 0, 0, 0 };
		// Set source offsets of offset 1 to mipwidth, mipheight and 1 for Z
		blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };

		// Set aspectmask of the source subresource to color bit
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		// Set miplevel of the source subresourde to i - 1
		blit.srcSubresource.mipLevel = i - 1;
		// Set the base array layer of the source subresource to 0
		blit.srcSubresource.baseArrayLayer = 0;
		// Set the layer count of the source subresource to 1
		blit.srcSubresource.layerCount = 1;

		// Set destination offsets of 0 to 0, 0, 0
		blit.dstOffsets[0] = { 0, 0, 0 };
		// Set destination offsets of 1:
		// If mipWidth is bigger than 1, set mipwidth/2 as offset, if not, set 1
		// If mipHeight is bigger than 1, set mipHeight/2 as offset, if not, set 1
		// Set 1 as offset for z
		blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };

		// Set aspect mask of the destination subresource to color
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		// Set miplevel of the destination subresource to i
		blit.dstSubresource.mipLevel = i;
		// Set base array layer of destination subresource to 0
		blit.dstSubresource.baseArrayLayer = 0;
		// Set layercount of destination subresource to 1
		blit.dstSubresource.layerCount = 1;

		// Blit the image
		vkCmdBlitImage(commandBuffer,
			image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &blit,
			VK_FILTER_LINEAR);

		// Set the old layout to transfer source optimal
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		// Set the new layout to shader read only optimal
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		// Set source access mask to transfer read
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		// Set destination access mask to shader read
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		// Set new barrier
		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		// If possible, divide mipwidth by 2
		if (mipWidth > 1) mipWidth /= 2;
		// If possible, divide mipheight by 2
		if (mipHeight > 1) mipHeight /= 2;
	}

	// Set base mip level level of subresource to miplevels -1
	barrier.subresourceRange.baseMipLevel = mipLevels - 1;
	// Set old layout to transfer destination optimal
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	// Set new layout to shader read optimal
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	// Set source access mask to transfer write
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	// Set destination access mask to shader read
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	// Set new barrier
	vkCmdPipelineBarrier(commandBuffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
		0, nullptr,
		0, nullptr,
		1, &barrier);
}

void D3D::ImageManager::CreateTextureImage(GPUObject* pGPUObject, D3D::BufferManager* pBufferManager, D3D::Texture& texture, const std::string& textureName, D3D::CommandpoolManager* pCommandPoolManager)
{
	// Get device
	auto device{ pGPUObject->GetDevice() };

	// Create ints for texture width, texture height and texture channels
	int texWidth, texHeight, texChannels;

	// Load pixels of image
	stbi_uc* pixels = stbi_load(textureName.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	// Calculate max amount of miplevels based on texwidth and texheight
	texture.mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

	// Calculate the image size
	VkDeviceSize imageSize = static_cast<uint64_t>(texWidth) * static_cast<uint64_t>(texHeight) * static_cast<uint64_t>(4);

	// If pixels didn't load correctly, throw runtime error
	if (!pixels)
	{
		throw std::runtime_error("failed to load texture image!");
	}

	// Create staging buffer object
	VkBuffer stagingBuffer{};
	// Create staging buffer memory object
	VkDeviceMemory stagingBufferMemory{};

	// Create the staging buffer
	pBufferManager->CreateBuffer(pGPUObject, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingBufferMemory);

	// Create void pointer to hold data
	void* data;

	// Map the memory of the staging buffer memory to the data pointer
	vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
	// Copy the data from the pixels to the data pointer
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	// Unmap the memory of the staging buffer
	vkUnmapMemory(device, stagingBufferMemory);

	// Free the pixels
	stbi_image_free(pixels);

	// Create the image
	CreateImage(pGPUObject, texWidth, texHeight, texture.mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		texture);

	// Create a command buffer object
	VkCommandBuffer commandBuffer{};

	// Get single time command buffer
	commandBuffer = pCommandPoolManager->BeginSingleTimeCommands(device);
	// Transition the image layout from undifined to transfer destination optimal
	TransitionImageLayout(texture.image, commandBuffer, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, texture.mipLevels);
	// End single time command buffer
	pCommandPoolManager->EndSingleTimeCommands(pGPUObject, commandBuffer);

	// Get new single time command buffer
	commandBuffer = pCommandPoolManager->BeginSingleTimeCommands(device);
	// Coppy staging buffer to texture image
	CopyBufferToImage(commandBuffer, stagingBuffer, texture.image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	// End single time command buffer
	pCommandPoolManager->EndSingleTimeCommands(pGPUObject, commandBuffer);

	// Destroy the staging buffer
	vkDestroyBuffer(device, stagingBuffer, nullptr);
	// Free the staging buffer memory
	vkFreeMemory(device, stagingBufferMemory, nullptr);

	// Get new single time command buffer
	commandBuffer = pCommandPoolManager->BeginSingleTimeCommands(device);
	// Generate mipmaps for the image
	GenerateMipmaps(pGPUObject->GetPhysicalDevice(), commandBuffer, texture.image, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, texture.mipLevels);
	// En single time command buffer
	pCommandPoolManager->EndSingleTimeCommands(pGPUObject, commandBuffer);
}

void D3D::ImageManager::CreateImage(GPUObject* pGPUObject, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, Texture& texture)
{
	// Get device
	auto device{ pGPUObject->GetDevice() };

	// Create image create info object
	VkImageCreateInfo imageInfo{};
	// Set type to image create info
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	// Set image type to 2D
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	// Set extent width to image width
	imageInfo.extent.width = width;
	// Set extent height to image height
	imageInfo.extent.height = height;
	// Set extent depth to 1
	imageInfo.extent.depth = 1;
	// Set miplevels to given miplevels
	imageInfo.mipLevels = mipLevels;
	// Set arraylayers to 1
	imageInfo.arrayLayers = 1;
	// Set format to requested format
	imageInfo.format = format;
	// Set tiling to requested tiling
	imageInfo.tiling = tiling;
	// Set inital layout to undefined
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	// Set usage to requested usage
	imageInfo.usage = usage;
	// Set sharing mode to mode exclusive
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	// Set samples to the requested number of samples
	imageInfo.samples = numSamples;
	// Set flags to 0
	imageInfo.flags = 0;

	// Create the image
	if (vkCreateImage(device, &imageInfo, nullptr, &texture.image) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to create image!");
	}

	// Create memory requirements object
	VkMemoryRequirements memRequirements;
	// Get memory requirements
	vkGetImageMemoryRequirements(device, texture.image, &memRequirements);

	// Create allocatoin info object
	VkMemoryAllocateInfo allocInfo{};
	// Set type to memory allocate info
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	// Set allocation size to memory requirements size
	allocInfo.allocationSize = memRequirements.size;
	// Set find memory type and set it as the type index
	allocInfo.memoryTypeIndex = VulkanUtils::FindMemoryType(pGPUObject->GetPhysicalDevice(), memRequirements.memoryTypeBits, properties);

	// Allocate the memory
	if (vkAllocateMemory(device, &allocInfo, nullptr, &texture.imageMemory) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to allocate image memory!");
	}

	// Bind the image memory
	vkBindImageMemory(device, texture.image, texture.imageMemory, 0);
}


void D3D::ImageManager::TransitionImageLayout(VkImage image, VkCommandBuffer commandBuffer, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, uint32_t layerCount)
{
	// Create memory barrier
	VkImageMemoryBarrier barrier{};

	// Set type to image memory barrier
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	// Set old layout to requested old layout
	barrier.oldLayout = oldLayout;
	// Set new layout to requested new layout
	barrier.newLayout = newLayout;
	// Set image to requested image
	barrier.image = image;

	// Set subresource aspectmask to color
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	// Set subresource basemiplevel to 0
	barrier.subresourceRange.baseMipLevel = 0;
	// Set subresource levelcount to the amount of miplevels
	barrier.subresourceRange.levelCount = mipLevels;
	// Set subresource base array layer to 0
	barrier.subresourceRange.baseArrayLayer = 0;
	// Set subresource layercount to 1
	barrier.subresourceRange.layerCount = layerCount;

	// Create source stage flags object
	VkPipelineStageFlags sourceStage{};
	// Create destination stage flags object
	VkPipelineStageFlags destinationStage{};

	// If new layout is depth stencil attachment optimal
	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		// Set subresource aspectmask to depth
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		
		// If the requested format has the stencil component
		if (HasStencilComponent(format))
		{
			// Add aspect stencil to the aspectmask
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else
	{
		// Set aspectmask to color
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	// If old layout is undifined and new layout is transfer destination optimal
	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		// Set source mask to 0
		barrier.srcAccessMask = 0;
		// Set destination mask to transfer write
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		// Set source stage to top of pipe
		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		// Set destination to transfer
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	// If old layout is transfer destination optimal and new layout is shader read only optimal
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		// Set source mask to transfer write
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		// Set destination mask to shader read
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		// Set source stage to transfer
		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		// Set destination to fragment shader
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	// If old layout is undifined and the new layout is depth stencil attachment
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		// Set source mask to 0
		barrier.srcAccessMask = 0;
		// Set destination mask to depth stencil attachent read and depth stencil attachment write
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		// Set source stage to top of pipe
		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		// Set destination stage to early fragment test
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = 0;

		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
	{
		// If nothing was done, throw invalid argument
		throw std::invalid_argument("unsupported layout transition!");
	}

	// Set new barrier
	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);
}

bool D3D::ImageManager::HasStencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}
