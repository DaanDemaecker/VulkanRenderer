// VulkanShaderModule.cpp

// Header include
#include "VulkanShaderModule.h"

// File includes
#include "Vulkan/Core/VulkanCore.h"
#include "Vulkan/Core/VulkanAllocator.h"
#include "Vulkan/Descriptors/VulkanDescriptorSetLayout.h"

#include "ServiceLocator/ServiceLocator.h"

// Standard library includes
#include <iostream>
#include <stdexcept>

DDM::Vulkan::VulkanShaderModule::VulkanShaderModule(const VulkanCore* pCore, const VulkanAllocator* pAllocator)
	:VulkanObject(pCore, pAllocator)
{

}

DDM::Vulkan::VulkanShaderModule::~VulkanShaderModule()
{
	if (m_VkShaderModule != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(m_pCore->GetDeviceHandle(), m_VkShaderModule, m_pAllocator->GetAllocator());
	}

	if (m_Reflected)
	{
		spvReflectDestroyShaderModule(&m_ReflectedModule);
	}
}

void DDM::Vulkan::VulkanShaderModule::CreateShaderModule(const std::string& filePath)
{
	ReadFromFile(filePath);

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.codeSize = m_ShaderCode.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(m_ShaderCode.data());

	if (vkCreateShaderModule(m_pCore->GetDeviceHandle(), &createInfo, m_pAllocator->GetAllocator(), &m_VkShaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create shader module");
	}
}

void DDM::Vulkan::VulkanShaderModule::AddDescriptorSetLayout(std::vector<std::unique_ptr<VulkanDescriptorSetLayout>>& descriptorSetLayouts)
{
	if (!m_Reflected)
	{
		Reflect();
	}

	descriptorSetLayouts.push_back(std::make_unique<VulkanDescriptorSetLayout>(m_pCore, m_pAllocator, this));
}

void DDM::Vulkan::VulkanShaderModule::AddPushConstantRanges(std::vector<VkPushConstantRange>& pushConstantRanges)
{
	if(!m_Reflected)
	{
		Reflect();
	}

	for (uint32_t i{}; i < m_ReflectedModule.push_constant_block_count; ++i)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = static_cast<VkShaderStageFlags>(m_ReflectedModule.shader_stage);
		pushConstantRange.offset = m_ReflectedModule.push_constant_blocks[i].offset;
		pushConstantRange.size = m_ReflectedModule.push_constant_blocks[i].size;
		pushConstantRanges.push_back(pushConstantRange);
	}
}

void DDM::Vulkan::VulkanShaderModule::SetupShaderStageInfo(VkPipelineShaderStageCreateInfo& stageInfo)
{
	stageInfo.stage = static_cast<VkShaderStageFlagBits>(m_ReflectedModule.shader_stage);
	stageInfo.module = m_VkShaderModule;
	stageInfo.pName = m_ReflectedModule.entry_point_name;

	for (uint32_t i{}; i < stageInfo.pSpecializationInfo->mapEntryCount; ++i)
	{
		auto currentEntry{ stageInfo.pSpecializationInfo->pMapEntries[i] };

		if (currentEntry.constantID >= m_ReflectedModule.spec_constant_count)
		{
			throw std::runtime_error("Invalid id was used to specify specilization constant");
		}
	}
}

void DDM::Vulkan::VulkanShaderModule::ReadFromFile(const std::string& filePath)
{
	if (!ServiceLocator::GetFileSystem().OpenRead(filePath))
	{
		return;
	}

	if (!ServiceLocator::GetFileSystem().ReadAll(filePath, m_ShaderCode))
	{
		std::cout << "Failed to read shader file: " << filePath << std::endl;
	}

	ServiceLocator::GetFileSystem().CloseRead(filePath);

	m_ShaderCodeRead = true;
}

void DDM::Vulkan::VulkanShaderModule::Reflect()
{
	if (spvReflectCreateShaderModule(m_ShaderCode.size(), m_ShaderCode.data(), &m_ReflectedModule) != SPV_REFLECT_RESULT_SUCCESS)
	{
		throw std::runtime_error("Failed to reflect shadermodule");
	}

	m_Reflected = true;
}
