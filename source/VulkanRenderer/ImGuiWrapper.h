// ImGuiWrapper.h
// This class wil function as a wrapper that handles the ImGui library

#ifndef ImGuiWrapperIncluded
#define ImGuiWrapperIncluded

#include "GLFWIncludes.h"
#include "ImGuiIncludes.h"

namespace D3D
{
	class ImGuiWrapper
	{
	public:
		ImGuiWrapper(ImGui_ImplVulkan_InitInfo init_info, VkRenderPass renderPass, VkCommandBuffer commandBuffer, VkDevice device, uint32_t maxFrames);

		void Cleanup(VkDevice device);

		void Render(VkCommandBuffer commandBuffer);

	private:
		VkDescriptorPool m_DescriptorPool{};

		void InitDescriptorPool(VkDevice device, uint32_t maxFrames);

	};
}
#endif // !ImGuiWrapperIncluded