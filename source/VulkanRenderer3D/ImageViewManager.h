// ImageViewManager.h
// This class will hold and manage the color- and depthImaveViews

//File includes
#include "VulkanIncludes.h"
#include "Structs.h"

namespace D3D
{
	// Class forward declaration for image manager
	class ImageManager;

	class ImageViewManager final
	{
	public:
		// Constructor
		// Parameters:
		//     msaaSamples: max amount of samples per pixel
		ImageViewManager(VkSampleCountFlagBits msaaSamples);

		// Delete default constructor
		ImageViewManager() = delete;

		// Dafault destructor
		~ImageViewManager() = default;

		ImageViewManager(ImageViewManager& other) = delete;
		ImageViewManager(ImageViewManager&& other) = delete;

		ImageViewManager& operator=(ImageViewManager& other) = delete;
		ImageViewManager& operator=(ImageViewManager&& other) = delete;

		// Clean up allocated objects
		// Parameters:
		//     device: handle to the VkDevice
		void Cleanup(VkDevice device);

		// Initialize the color image for the swapchain
		// Parameters:
		//     device: handle of the VkDevice
		//     physicalDevice: handle of the VkPhysicalDevice
		//     format: the requested format for the color image
		//     swapchainExtent: the extent of the swapchain, for use of the image size
		//     pImageManager: pointer to the image manager
		void CreateColorResources(VkDevice device, VkPhysicalDevice physicalDevice, VkFormat format,
			VkExtent2D swapchainExtent, D3D::ImageManager* pImageManager);
		// Initialize the depth image for the swapchain
		// Parameters:
		//     device: handle of the VkDevice
		//     physicalDevice: handle of the VkPhysicalDevice
		//     swapchainExtent: the extent of the swapchain, for use of the image size
		//     pImageManager: pointer to the image manager
		void CreateDepthResources(VkDevice device, VkPhysicalDevice physicalDevice,
			VkExtent2D swapchainExtent, D3D::ImageManager* pImageManager, VkCommandBuffer commandBuffer);

		// Get the imageView for the color image
		VkImageView GetColorImageView() const { return m_ColorImage.imageView; }
		// Get the imageView for the depth image
		VkImageView GetDepthImageView() const { return m_DepthImage.imageView; }
		// Get the max amount of samples per pixel
		VkSampleCountFlagBits GetMsaaSamples() const { return m_MsaaSamples; }

	private:
		// Max amount of samples per pixel, initialize as 1
		VkSampleCountFlagBits m_MsaaSamples = VK_SAMPLE_COUNT_1_BIT;

		// Texture struct for the color image
		Texture m_ColorImage{};

		// Texture struct for the depth image
		Texture m_DepthImage{};
	};
}