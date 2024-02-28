// ImageViewManager.h
// This class will hold and manage the color- and depthImaveViews

//File includes
#include "VulkanIncludes.h"
#include "Structs.h"

namespace D3D
{
	class ImageManager;

	class ImageViewManager
	{
	public:
		ImageViewManager(VkSampleCountFlagBits msaaSamples);
		~ImageViewManager() = default;

		ImageViewManager(ImageViewManager& other) = delete;
		ImageViewManager(ImageViewManager&& other) = delete;

		ImageViewManager& operator=(ImageViewManager& other) = delete;
		ImageViewManager& operator=(ImageViewManager&& other) = delete;

		void Cleanup(VkDevice device);

		void CreateColorResources(VkDevice device, VkPhysicalDevice physicalDevice, VkFormat format, VkExtent2D swapchainExtent, D3D::ImageManager* pImageManager);
		void CreateDepthResources(VkDevice device, VkPhysicalDevice physicalDevice, VkFormat depthFormat, VkExtent2D swapchainExtent, D3D::ImageManager* pImageManager, VkCommandBuffer commandBuffer);

		VkImageView GetColorImageView() const { return m_ColorImage.imageView; }
		VkImageView GetDepthImageView() const { return m_DepthImage.imageView; }
		VkSampleCountFlagBits GetMsaaSamples() const { return m_MsaaSamples; }

	private:
		//--MultiSampling--
		VkSampleCountFlagBits m_MsaaSamples = VK_SAMPLE_COUNT_1_BIT;
		Texture m_ColorImage{};

		//--Depth Image--
		Texture m_DepthImage{};
	};
}