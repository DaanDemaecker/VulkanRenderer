// ImageManager.h
// This class will handle everything having to do with images

#ifndef ImageManagerIncluded
#define ImageManagerIncluded

// File includes
#include "GLFWIncludes.h"

// Standard library includes
#include <iostream>

namespace D3D
{
	class ImageManager
	{
	public:
		ImageManager(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandBuffer commandBuffer);
		
		~ImageManager() = default;

		ImageManager(ImageManager& other) = delete;
		ImageManager(ImageManager&& other) = delete;

		ImageManager& operator=(ImageManager& other) = delete;
		ImageManager& operator=(ImageManager&& other) = delete;

		void Cleanup(VkDevice device);

		VkImageView& GetDefaultImageView() { return m_DefaultTextureImageView; }
		VkSampler& GetSampler() { return m_TextureSampler; }

		void CopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

		void GenerateMipmaps(VkPhysicalDevice physicalDevice, VkCommandBuffer commandBuffer,
			VkImage image, VkFormat imageFormat,
			int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

		VkImageView CreateImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
		
		void TransitionImageLayout(VkImage image, VkCommandBuffer commandBuffer, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

		void CreateBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	private:

		VkImage m_DefaultTextureImage{};
		VkDeviceMemory m_DefaultTextureImageMemory{};
		VkImageView m_DefaultTextureImageView{};
		uint32_t m_MipLevels{};

		const std::string m_DefaultTextureName{ "../resources/DefaultResources/DefaultTexture.png" };


		VkSampler m_TextureSampler{};

		void CreateDefaultTextureImage(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandBuffer commandBuffer);

		void CreateDefaultImageView(VkDevice device);
		void CreateTextureSampler(VkDevice device, VkPhysicalDevice physicalDevice);

		void CreateImage(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
		
		bool HasStencilComponent(VkFormat format);
	};
}
#endif // !ImageManagerIncluded