// RenerpassWrapper.h
// This class will serve as a wrapper for the VkRenderPass object

#ifndef RenderpassWrapperIncluded
#define RenderpassWrapperIncluded

// File includdes
#include "Includes/VulkanIncludes.h"

namespace DDM3
{
	class RenderpassWrapper final
	{
	public:
		// Constructor
		// Parameters:
		//     device: handle of the VkDevice
		//     swapchainImageFormat: the format that the swapchain color image is in
		//     depthFormat: the format that the swapchain depth image is in
		//     msaaSamples: the max amount of samples per pixels
		RenderpassWrapper(VkDevice device, VkFormat swapchainImageFormat, VkFormat depthFormat,
			VkSampleCountFlagBits msaaSamples);

		// Delete default constructor
		RenderpassWrapper() = delete;

		// Destructor
		~RenderpassWrapper();

		// Delete copy and move functions
		RenderpassWrapper(RenderpassWrapper& other) = delete;
		RenderpassWrapper(RenderpassWrapper&& other) = delete;
		RenderpassWrapper& operator=(RenderpassWrapper& other) = delete;
		RenderpassWrapper& operator=(RenderpassWrapper&& other) = delete;

		// Get the handle of the renderpass
		VkRenderPass GetRenderpass() const { return m_RenderPass; }

		void BeginRenderPass(VkCommandBuffer commandBuffer, VkFramebuffer frameBuffer, VkExtent2D swapchainExtent);

	private:

		//Renderpass
		VkRenderPass m_RenderPass{};

		//RenderpassInfo
		VkRenderPassBeginInfo m_RenderpassInfo{};

		// Initialize the renderpass
		// Parameters:
		//     device: handle of the VkDevice
		//     swapchainImageFormat: the format that the swapchain color image is in
		//     depthFormat: the format that the swapchain depth image is in
		//     msaaSamples: the max amount of samples per pixels
		void CreateRenderPass(VkDevice device, VkFormat swapchainImageFormat, VkFormat depthFormat,
			VkSampleCountFlagBits msaaSamples);

		// Clean up allocated objects
		// Parameters:
		//     device: handle of the VkDevice
		void Cleanup(VkDevice device);
	};
}

#endif // !RenderpassWrapperIncluded
