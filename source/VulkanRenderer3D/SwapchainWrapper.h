// SwapchainWrapper.h
// This class will serve as a wrapper for the vulkan swapchain

#ifndef SwapchainWrapperIncluded
#define SwapchainWrapperIncluded

// File includes
#include "VulkanIncludes.h"
#include "Structs.h"

// Standard library includes
#include <memory>

namespace D3D
{
	// Class forward declarations
	class ImageManager;
	class ImageViewManager;
	class GPUObject;

	class SwapchainWrapper final
	{
	public:
		// Constructor
		// Parameter:
		//     pGPUObject: pointer to the GPUObject
		//     surface: handle of the VkSurfaceKHR
		//     pImageManager: pointer to the image manager
		//     msaaSamples: max amount of samples per pixel
		SwapchainWrapper(GPUObject* pGPUObject, VkSurfaceKHR surface, D3D::ImageManager* pImageManager, VkSampleCountFlagBits msaaSamples);

		// Delete default constructor
		SwapchainWrapper() = delete;

		// Destructor
		~SwapchainWrapper();

		// Delete copy and move functions
		SwapchainWrapper(SwapchainWrapper& other) = delete;
		SwapchainWrapper(SwapchainWrapper&& other) = delete;
		SwapchainWrapper& operator=(SwapchainWrapper& other) = delete;
		SwapchainWrapper& operator=(SwapchainWrapper&& other) = delete;

		// Set up the color and depth image views
		// Parameters:
		//     pGPUObject: pointer to the GPUObject
		//     pImageManager: pointer to the image manager
		//     commandBuffer: commandbuffer that will be used to create depth image
		//     renderPass: the renderpass that will be used with this swapchain
		void SetupImageViews(GPUObject* pGPUObject, D3D::ImageManager* pImageManager,
			VkCommandBuffer commandBuffer, VkRenderPass renderPass);

		// Clean up allocated objects
		// Parameters:
		//     device: handle of the VkDevice
		void Cleanup(VkDevice device);

		// Delete and recreate the swapchain
		// Parameters:
		//     pGPUObject: pointer to the GPUObject
		//     surface: handle of the VkSurfaceKHR
		//     pImageManager: pointer to the image manager
		//     commandBuffer: commandbuffer that will be used to create depth image
		//     renderpass: the renderpass that will be used with this swapchain
		void RecreateSwapChain(GPUObject* pGPUObject, VkSurfaceKHR surface,
			D3D::ImageManager* pImageManager, VkCommandBuffer commandBuffer, VkRenderPass renderpass);

		// Get the swapchain
		VkSwapchainKHR GetSwapchain() const { return m_SwapChain; }

		// Get the format of the swapchain
		VkFormat GetFormat() const { return m_SwapChainImageFormat; }

		// Get the minimum image count
		uint32_t GetMinImageCount() const { return m_MinImageCount; }

		// Get the extent of the swapchain
		VkExtent2D GetExtent() const { return m_SwapChainExtent; }

		// Get the requested frame buffer
		// Parameters:
		//     index: the index of the frame buffer
		VkFramebuffer GetFrameBuffer(uint32_t index) const { return m_SwapChainFramebuffers[index]; }

		// Get the amound of samples per pixel
		VkSampleCountFlagBits GetMsaaSamples() const;
	private:
		// The image view manager that hold the color and depth image
		std::unique_ptr<ImageViewManager> m_pImageViewManager{};

		// Handle of the swapchaint
		VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;

		// The minimum amount of images
		uint32_t m_MinImageCount{};

		// Vector of swapchain images
		std::vector<VkImage> m_SwapChainImages{};

		// Vector of swapchain imageviews
		std::vector<VkImageView> m_SwapChainImageViews{};

		// Format of the swapchain
		VkFormat m_SwapChainImageFormat{};

		// Extent of the swapchain
		VkExtent2D m_SwapChainExtent{};

		// Vector of frameBuffers
		std::vector<VkFramebuffer> m_SwapChainFramebuffers{};

		// Initialize the swapchain and other components
		// Parameters:
		//     pGPUObject: pointer to the GPUObject
		//     surface: Handle of the VkSurfaceKHR
		//     pImageManager: pointer to the image manager
		//     commandBuffer: commandbuffer needed for creation of depth image
		//     renderpass: the renderpass used with this swapchain
		void SetupSwapchain(GPUObject* pGPUObject, VkSurfaceKHR surface,
			D3D::ImageManager* pImageManager, VkCommandBuffer commandBuffer, VkRenderPass renderpass);

		// Create the swapchain
		// Parameters:
		//     pGPUObject: pointer to the GPUObject
		//     surface: handle of the VkSurfaceKHR
		void CreateSwapChain(GPUObject* pGPUObject, VkSurfaceKHR surface);

		// Create the color and depth image views
		// Parameters:
		//     device: handle of the VkDevice
		//     pImageManager: handle of the image manager
		void CreateSwapchainImageViews(VkDevice device, ImageManager* pImageManager);

		// Create the frame buffers
		// Parameters:
		//     device: handle of the VkDevice
		void CreateFramebuffers(VkDevice device, VkRenderPass renderpass);

		// Get the format for the swapchain surface
		// Parameters:
		//     availableFormats: list of candidate formats for the swapchain
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

		// Choose the extent of the swapchain
		// Parameters:
		//     capabilities: the needed capabilities for the swapchain
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		// Choose the presentmode of the swapchain
		// Parameters:
		//     availablePresentModes: list of candidate presentmodes for the swapchain
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	};

}
#endif // !SwapchainWrapperIncluded