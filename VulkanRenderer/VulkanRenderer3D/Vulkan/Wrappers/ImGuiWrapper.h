// ImGuiWrapper.h
// This class wil function as a wrapper that handles the ImGui library

#ifndef ImGuiWrapperIncluded
#define ImGuiWrapperIncluded

// File includes
#include "Includes/VulkanIncludes.h"
#include "Includes/ImGuiIncludes.h"

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
		ImGuiWrapper(ImGui_ImplVulkan_InitInfo init_info, VkDevice device);

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

		// Set up the imgui frame
		void StartRender();

		// Render the imgui frame
		// Parameters:
		//     commandBuffer: the commandBuffer used for rendering
		void EndRender(VkCommandBuffer commandBuffer);

	private:
		// Descriptorpool needed for ImGUI
		VkDescriptorPool m_DescriptorPool{};

		// Initialization of descriptorpool
		// Parameters:
		//     device: handle for the VkDevice
		void InitDescriptorPool(VkDevice device);

	};
}
#endif // !ImGuiWrapperIncluded