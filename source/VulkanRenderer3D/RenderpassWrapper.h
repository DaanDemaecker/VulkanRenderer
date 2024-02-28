// RenerpassWrapper.h
// This class will serve as a wrapper for the VkRenderPass object

#ifndef RenderpassWrapperIncluded
#define RenderpassWrapperIncluded

// File includdes
#include "VulkanIncludes.h"

namespace D3D
{
	class RenderpassWrapper
	{
	public:
		RenderpassWrapper(VkDevice device, VkFormat swapchainImageFormat, VkFormat depthFormat,
			VkSampleCountFlagBits msaaSamples);

		RenderpassWrapper() = delete;
		~RenderpassWrapper() = default;

		RenderpassWrapper(RenderpassWrapper& other) = delete;
		RenderpassWrapper(RenderpassWrapper&& other) = delete;

		RenderpassWrapper& operator=(RenderpassWrapper& other) = delete;
		RenderpassWrapper& operator=(RenderpassWrapper&& other) = delete;
	
		void cleanup(VkDevice device);

		VkRenderPass GetRenderpass() const { return m_RenderPass; }

	private:

		//Renderpass
		VkRenderPass m_RenderPass{};

		//RenderpassInfo
		VkRenderPassBeginInfo m_RenderpassInfo{};

		void CreateRenderPass(VkDevice device, VkFormat swapchainImageFormat, VkFormat depthFormat,
			VkSampleCountFlagBits msaaSamples);

	};
}

#endif // !RenderpassWrapperIncluded
