// VulkanPipelineLayout.cpp

// Header include
#include "VulkanPipelineLayout.h"

// File includes
#include "Vulkan/Descriptors/VulkanDescriptorSetLayout.h"
#include "Vulkan/Shaders/VulkanShaderModule.h"

DDM::Vulkan::VulkanPipelineLayout::VulkanPipelineLayout(const VulkanCore* pCore, const VulkanAllocator* pAllocator, std::vector<std::unique_ptr<VulkanShaderModule>>& shaderModules)
	:VulkanObject(pCore, pAllocator)
{
	CreateLayout(shaderModules);
}

DDM::Vulkan::VulkanPipelineLayout::~VulkanPipelineLayout()
{
	if (m_VkPipelineLayout != VK_NULL_HANDLE)
	{
		vkDestroyPipelineLayout(m_pCore->GetDeviceHandle(), m_VkPipelineLayout, m_pAllocator->GetAllocator());
	}
}

VkPipelineLayout DDM::Vulkan::VulkanPipelineLayout::GetPipelineLayout() const
{
	return m_VkPipelineLayout;
}

void DDM::Vulkan::VulkanPipelineLayout::CreateLayout(std::vector<std::unique_ptr<VulkanShaderModule>>& shaderModules)
{
	m_DescriptorSetLayouts.clear();
	std::vector<VkPushConstantRange> pushConstantRanges{};

	VkPipelineLayoutCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;

	for (auto& shaderModule : shaderModules)
	{
		shaderModule->AddDescriptorSetLayout(m_DescriptorSetLayouts);
		shaderModule->AddPushConstantRanges(pushConstantRanges);
	}

	std::vector<VkDescriptorSetLayout> vkDescriptorSetLayouts(m_DescriptorSetLayouts.size());

	for (uint32_t i{}; i < m_DescriptorSetLayouts.size(); ++i)
	{
		vkDescriptorSetLayouts[i] = m_DescriptorSetLayouts[i]->GetDescriptorSetLayout();
	}

	createInfo.setLayoutCount = static_cast<uint32_t>(vkDescriptorSetLayouts.size());
	createInfo.pSetLayouts = vkDescriptorSetLayouts.data();

	createInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
	createInfo.pPushConstantRanges = pushConstantRanges.data();

	VkResult result = vkCreatePipelineLayout(m_pCore->GetDeviceHandle(), &createInfo, m_pAllocator->GetAllocator(), &m_VkPipelineLayout);
		
	if(result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create pipeline layout");
	}
}
