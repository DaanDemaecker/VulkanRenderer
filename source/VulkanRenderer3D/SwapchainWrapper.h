// SwapchainWrapper.h
// This class will serve as a wrapper for the vulkan swapchain

#ifndef SwapchainWrapperIncluded
#define SwapchainWrapperIncluded

// File includes
#include "ImageViewManager.h"
#include "VulkanIncludes.h"
#include "Structs.h"

// Standard library includes
#include <memory>

namespace D3D
{
	class ImageManager;

	class SwapchainWrapper
	{
	public:
		SwapchainWrapper(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
			D3D::ImageManager* pImageManager, VkSampleCountFlagBits msaaSamples);
		~SwapchainWrapper() = default;

		SwapchainWrapper(SwapchainWrapper& other) = delete;
		SwapchainWrapper(SwapchainWrapper&& other) = delete;

		SwapchainWrapper& operator=(SwapchainWrapper& other) = delete;
		SwapchainWrapper& operator=(SwapchainWrapper&& other) = delete;

		void SetupImageViews(VkDevice device, VkPhysicalDevice physicalDevice, D3D::ImageManager* pImageManager,
			VkFormat depthFormat, VkCommandBuffer commandBuffer, VkRenderPass renderPass);

		void Cleanup(VkDevice device);

		void RecreateSwapChain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
			D3D::ImageManager* pImageManager, VkCommandBuffer commandBuffer, VkFormat depthFormat,
			VkRenderPass renderpass);

		VkSwapchainKHR GetSwapchain() const { return m_SwapChain; }
		VkFormat GetFormat() const { return m_SwapChainImageFormat; }
		uint32_t GetMinImageCount() const { return m_MinImageCount; }
		VkExtent2D GetExtent() const { return m_SwapChainExtent; }
		VkFramebuffer GetFrameBuffer(uint32_t index) const { return m_SwapChainFramebuffers[index]; }

		VkSampleCountFlagBits GetMsaaSamples() const { return m_pImageViewManager->GetMsaaSamples(); }
	private:
		std::unique_ptr<ImageViewManager> m_pImageViewManager{};

		//--Swapchain--
		VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;

		//-Swapchain MinImageCount-
		uint32_t m_MinImageCount{};

		//-Swapchain Images-
		std::vector<VkImage> m_SwapChainImages{};

		//-Swapchain Image Format-
		VkFormat m_SwapChainImageFormat{};

		//-Swapchain Extent-
		VkExtent2D m_SwapChainExtent{};

		//-Image Views-
		std::vector<VkImageView> m_SwapChainImageViews{};

		//--Framebuffers--
		std::vector<VkFramebuffer> m_SwapChainFramebuffers{};


		void SetupSwapchain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
			D3D::ImageManager* pImageManager, VkCommandBuffer commandBuffer, VkFormat depthFormat, VkRenderPass renderpass);

		void CreateSwapChain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

		void CreateImageViews(VkDevice device, ImageManager* pmageManager);

		void CreateFramebuffers(VkDevice device, VkRenderPass renderpass);

		D3D::SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		D3D::QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
	};

}
#endif // !SwapchainWrapperIncluded