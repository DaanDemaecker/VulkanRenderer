// VulkanDescriptorSetLayout.cpp

// Header include
#include "VulkanDescriptorSetLayout.h"

// File includes
#include "Vulkan/Shaders/VulkanShaderModule.h"

DDM::Vulkan::VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(const VulkanCore* pCore, const VulkanAllocator* pAllocator, const VulkanShaderModule* pShaderModule)
	:VulkanObject(pCore, pAllocator)
{
	CreateDescriptorSetLayout(pShaderModule);
}

DDM::Vulkan::VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
{
	if (m_VkDescriptorSetLayout != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorSetLayout(m_pCore->GetDeviceHandle(), m_VkDescriptorSetLayout, m_pAllocator->GetAllocator());
	}
}

void DDM::Vulkan::VulkanDescriptorSetLayout::CreateDescriptorSetLayout(const VulkanShaderModule* pShaderModule)
{
	VkDescriptorSetLayoutCreateInfo createInfo{};

	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;

	std::vector<VkDescriptorSetLayoutBinding> bindings{};

	ReadBindings(pShaderModule->GetReflectedModule(), bindings);

	createInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	createInfo.pBindings = bindings.data();


	if (vkCreateDescriptorSetLayout(m_pCore->GetDeviceHandle(), &createInfo, m_pAllocator->GetAllocator(), &m_VkDescriptorSetLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create descriptor set layout");
	}
}

void DDM::Vulkan::VulkanDescriptorSetLayout::ReadBindings(const SpvReflectShaderModule& pShaderModule, std::vector<VkDescriptorSetLayoutBinding>& bindings)
{
	for(uint32_t i{}; i < pShaderModule.descriptor_binding_count; ++i)
	{
		auto currentBinding = pShaderModule.descriptor_bindings[i];
		
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = currentBinding.binding;
		layoutBinding.descriptorType = static_cast<VkDescriptorType>(currentBinding.descriptor_type);
		layoutBinding.descriptorCount = currentBinding.count;
		layoutBinding.stageFlags = static_cast<VkShaderStageFlags>(pShaderModule.shader_stage);
		layoutBinding.pImmutableSamplers = nullptr;


		bindings.push_back(layoutBinding);
	}
}
