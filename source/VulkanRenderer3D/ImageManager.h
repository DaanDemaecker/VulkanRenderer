// ImageManager.h
// This class will handle everything having to do with images

#ifndef ImageManagerIncluded
#define ImageManagerIncluded

// File includes
#include "VulkanIncludes.h"
#include "Structs.h"
#include "CommandpoolManager.h"

// Standard library includes
#include <iostream>

namespace D3D
{
	class ImageManager
	{
	public:
		ImageManager(VkDevice device, VkPhysicalDevice physicalDevice, CommandpoolManager* pCommandPoolManager, VkQueue graphicsQueue);
		
		~ImageManager() = default;

		ImageManager(ImageManager& other) = delete;
		ImageManager(ImageManager&& other) = delete;

		ImageManager& operator=(ImageManager& other) = delete;
		ImageManager& operator=(ImageManager&& other) = delete;

		void Cleanup(VkDevice device);

		VkImageView& GetDefaultImageView() { return m_DefaultTexture.imageView; }
		VkSampler& GetSampler() { return m_TextureSampler; }

		void CopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

		void GenerateMipmaps(VkPhysicalDevice physicalDevice, VkCommandBuffer commandBuffer,
			VkImage image, VkFormat imageFormat,
			int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

		void CreateTextureImage(VkDevice device, VkPhysicalDevice physicalDevice, Texture& texture, const std::string& textureName, uint32_t& miplevels, CommandpoolManager* pCommandPoolManager, VkQueue graphicsQueue);
		void CreateTextureSampler(VkSampler& sampler, uint32_t samples, VkDevice device, VkPhysicalDevice physicalDevice);
		VkImageView CreateImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
		
		void TransitionImageLayout(VkImage image, VkCommandBuffer commandBuffer, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

		void CreateImage(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, Texture& texture);

	private:
		Texture m_DefaultTexture{};

		uint32_t m_MipLevels{};

		const std::string m_DefaultTextureName{ "../resources/DefaultResources/DefaultTexture.png" };


		VkSampler m_TextureSampler{};

		bool HasStencilComponent(VkFormat format);

		void CreateDefaultResources(VkDevice device, VkPhysicalDevice physicalDevice,  CommandpoolManager* pCommandPoolManager, VkQueue graphicsQueue);
	};
}
#endif // !ImageManagerIncluded