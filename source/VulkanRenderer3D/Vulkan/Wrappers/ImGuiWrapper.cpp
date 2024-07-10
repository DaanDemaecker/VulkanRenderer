// ImGuiWrapper.cpp

// File includes
#include "ImGuiWrapper.h"
#include "../../Engine/Window.h"

// Standard library includes
#include <array>
#include <stdexcept>

D3D::ImGuiWrapper::ImGuiWrapper(ImGui_ImplVulkan_InitInfo init_info, VkRenderPass renderPass, VkCommandBuffer commandBuffer, VkDevice device, uint32_t maxFrames)
{	
	// Initialize the descriptorPool
	InitDescriptorPool(device, maxFrames);

	// Check ImGui version
	IMGUI_CHECKVERSION();

	// Create ImGui context
	ImGui::CreateContext();

	// Initialize ImGui Vulkan backend
	ImGui_ImplGlfw_InitForVulkan(Window::GetInstance().GetWindowStruct().pWindow, true);

	// Set the descriptorpool of the init_info
	init_info.DescriptorPool = m_DescriptorPool;

	// Init ImGui with init info and the renderpass
	ImGui_ImplVulkan_Init(&init_info, renderPass);

	// Create the fonts and textures with the commandbuffer
	ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
}

void D3D::ImGuiWrapper::Cleanup(VkDevice device)
{
	// Shut down vulkan implementation
	ImGui_ImplVulkan_Shutdown();

	// Shut down glfw implementation
	ImGui_ImplGlfw_Shutdown();

	// Destroy context
	ImGui::DestroyContext();

	// Destroy the descriptorpool
	vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);
}

void D3D::ImGuiWrapper::StartRender()
{
	// Start ImGui frame
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Show the demo window
	ImGui::ShowDemoWindow();

	
}

void D3D::ImGuiWrapper::EndRender(VkCommandBuffer commandBuffer)
{
	// Render frame
	ImGui::Render();

	// Record ImGui draw data
	ImDrawData* draw_data = ImGui::GetDrawData();

	// Render the draw data
	ImGui_ImplVulkan_RenderDrawData(draw_data, commandBuffer);
}

void D3D::ImGuiWrapper::InitDescriptorPool(VkDevice device, uint32_t maxFrames)
{
	// Create poolsizes array
	std::array<VkDescriptorPoolSize, 2> poolSizes{};

	// Set type of poolsize 0 to uniform buffer
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	// Set descriptorcount of poolsize 0 to amount of max frames
	poolSizes[0].descriptorCount = static_cast<uint32_t>(maxFrames);

	// Set type of poolsize 1 to combined image sampler
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	// Set descriptorcount of poolsize 1 to the amount of frames
	poolSizes[1].descriptorCount = static_cast<uint32_t>(maxFrames);

	// Create descriptorpool create info
	VkDescriptorPoolCreateInfo poolInfo{};
	// Set type to descriptor pool create info
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	// Set pool size count to the size of PoolSizes
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	// Give pointer to poolsizes data
	poolInfo.pPoolSizes = poolSizes.data();
	// Set max sets to max amount of frames
	poolInfo.maxSets = static_cast<uint32_t>(maxFrames);

	// Create descriptorpool, if failed throw runtime error
	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}
