// ImGuiWrapper.h
// This class wil function as a wrapper that handles the ImGui library

#ifndef ImGuiWrapperIncluded
#define ImGuiWrapperIncluded

// File includes
#include "VulkanIncludes.h"
#include "ImGuiIncludes.h"

namespace D3D
{
	class ImGuiWrapper final
	{
	public:
		// Constructor
		// Parameters:
		//     init_info: the initialization info for imgui
		//     renderpass: the handle to the renderpass that will be used
		//     commandbuffer: a single time command buffer needed for initialization
		//     device: the handle for the VkDevice
		//     maxFrames: the max frames in flight
		ImGuiWrapper(ImGui_ImplVulkan_InitInfo init_info, VkRenderPass renderPass, VkCommandBuffer commandBuffer,
			VkDevice device, uint32_t maxFrames);

		// Default destructor
		~ImGuiWrapper() = default;

		// Delete default constructor
		ImGuiWrapper() = delete;

		// Delete copy and move functions
		ImGuiWrapper(ImGuiWrapper& other) = delete;
		ImGuiWrapper(ImGuiWrapper&& other) = delete;
		ImGuiWrapper& operator=(ImGuiWrapper& other) = delete;
		ImGuiWrapper& operator=(ImGuiWrapper&& other) = delete;

		// Cleanup
		// Parameters:
		//     device: handle for the VkDevice
		void Cleanup(VkDevice device);

		// Render the imgui
		// Parameters:
		//     commandBuffer: the commandBuffer used for rendering
		void Render(VkCommandBuffer commandBuffer);

	private:
		// Descriptorpool needed for ImGUI
		VkDescriptorPool m_DescriptorPool{};

		// Initialization of descriptorpool
		// Parameters:
		//     device: handle for the VkDevice
		//     maxFrames: the amount of frames in flight
		void InitDescriptorPool(VkDevice device, uint32_t maxFrames);

	};
}
#endif // !ImGuiWrapperIncluded