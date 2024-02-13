// ImGuiWrapper.cpp

// File includes
#include "ImGuiWrapper.h"
#include "D3DEngine.h"

// Standard library includes
#include <array>
#include <stdexcept>

extern D3D::Window g_pWindow;

D3D::ImGuiWrapper::ImGuiWrapper(ImGui_ImplVulkan_InitInfo init_info, VkRenderPass renderPass, VkCommandBuffer commandBuffer, VkDevice device, uint32_t maxFrames)
{
	InitDescriptorPool(device, maxFrames);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Initialize ImGui Vulkan backend
	ImGui_ImplGlfw_InitForVulkan(g_pWindow.pWindow, true);

	init_info.DescriptorPool = m_DescriptorPool;
	ImGui_ImplVulkan_Init(&init_info, renderPass); // Your Vulkan render pass

	ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
}

void D3D::ImGuiWrapper::Cleanup(VkDevice device)
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);
}

void D3D::ImGuiWrapper::Render(VkCommandBuffer commandBuffer)
{
	// Start ImGui frame
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();

	// End ImGui frame
	ImGui::Render();

	// Record ImGui draw data
	ImDrawData* draw_data = ImGui::GetDrawData();
	ImGui_ImplVulkan_RenderDrawData(draw_data, commandBuffer);
}

void D3D::ImGuiWrapper::InitDescriptorPool(VkDevice device, uint32_t maxFrames)
{
	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(maxFrames);
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(maxFrames);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(maxFrames);

	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}
