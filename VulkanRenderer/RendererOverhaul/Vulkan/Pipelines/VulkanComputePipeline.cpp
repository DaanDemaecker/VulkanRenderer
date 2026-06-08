// VulkanComputePipeline.h

// Header include
#include "VulkanComputePipeline.h"

// File includes
#include "Vulkan/Shaders/VulkanShaderModule.h"
#include "Vulkan/Pipelines/VulkanPipelineLayout.h"
#include "Vulkan/Pipelines/VulkanSpecInfo.h"

// Standard library includes
#include <algorithm>
#include <iostream>

DDM::Vulkan::VulkanComputePipeline::VulkanComputePipeline(const VulkanCore* pCore, const VulkanAllocator* pAllocator)
	:VulkanObject(pCore, pAllocator)
{

}

DDM::Vulkan::VulkanComputePipeline::~VulkanComputePipeline()
{
	if (m_VkPipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(m_pCore->GetDeviceHandle(), m_VkPipeline, m_pAllocator->GetAllocator());
	}
}

void DDM::Vulkan::VulkanComputePipeline::AddShader(const std::string& shaderPath)
{
	if (shaderPath.empty())
	{
		std::cout << "No shaderpath given \n";
		return;
	}
	else if (m_ShaderPath.compare(""))
	{
		std::cout << "Overriding shaderpath: " << m_ShaderPath << " with: " << shaderPath << std::endl;
	}
	m_ShaderPath = shaderPath;
}

void DDM::Vulkan::VulkanComputePipeline::CreatePipeline()
{
	if (m_ShaderPath.empty())
	{
		throw std::runtime_error("No shaders added to the compute pipeline!");
	}

	std::vector<std::unique_ptr<VulkanShaderModule>> shaderModules(1);
	shaderModules[0] = std::make_unique<VulkanShaderModule>(m_pCore, m_pAllocator);
	shaderModules[0]->CreateShaderModule(m_ShaderPath);
	
	std::unique_ptr<VulkanPipelineLayout> layout{ std::make_unique<VulkanPipelineLayout>(m_pCore, m_pAllocator, shaderModules)};

	VkComputePipelineCreateInfo createInfo{};

	createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;

	VkSpecializationInfo specializationInfo{};

	if (m_pSpecializationInfo != nullptr)
	{
		m_pSpecializationInfo->FillSpecInfo(specializationInfo);
	}

	createInfo.stage.pSpecializationInfo = &specializationInfo;

	SetupStageInfo(shaderModules[0].get(), createInfo.stage);

	createInfo.layout = layout->GetPipelineLayout();
	createInfo.basePipelineHandle = VK_NULL_HANDLE;
	createInfo.basePipelineIndex = -1;


	if (vkCreateComputePipelines(m_pCore->GetDeviceHandle(), VK_NULL_HANDLE, 1, &createInfo, m_pAllocator->GetAllocator(), &m_VkPipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create compute pipeline!");
	}
}

void DDM::Vulkan::VulkanComputePipeline::SetSpecInfo(std::unique_ptr<VulkanSpecInfo> specInfo)
{
	m_pSpecializationInfo = std::move(specInfo);
}

void DDM::Vulkan::VulkanComputePipeline::SetupStageInfo(VulkanShaderModule* shaderModule, VkPipelineShaderStageCreateInfo& stageInfo)
{
	stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stageInfo.pNext = nullptr;
	shaderModule->SetupShaderStageInfo(stageInfo);
}
