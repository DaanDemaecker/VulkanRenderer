// VulkanComputePipeline.h

// Header include
#include "VulkanComputePipeline.h"

// File includes
#include "Vulkan/Shaders/VulkanShaderModule.h"

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
	if(std::find(m_ShaderPaths.begin(), m_ShaderPaths.end(), shaderPath) == m_ShaderPaths.end())
	{
		m_ShaderPaths.push_back(shaderPath);
	}
	else
	{
		std::cout << "Shader: " << shaderPath << " already added to compute pipeline" << std::endl;
	}
}

void DDM::Vulkan::VulkanComputePipeline::CreatePipeline()
{
	if (m_ShaderPaths.empty())
	{
		throw std::runtime_error("No shaders added to the compute pipeline!");
	}

	std::vector<std::unique_ptr<VulkanShaderModule>> shaderModules{ };

	shaderModules.reserve(m_ShaderPaths.size());

	for (auto& shaderPath : m_ShaderPaths)
	{
		shaderModules.push_back(std::make_unique<VulkanShaderModule>(m_pCore, m_pAllocator));
		shaderModules.back()->CreateShaderModule(shaderPath);
	}

	VkComputePipelineCreateInfo createInfo{};

	createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;

	createInfo.basePipelineHandle = VK_NULL_HANDLE;
	createInfo.basePipelineIndex = -1;


	if (vkCreateComputePipelines(m_pCore->GetDeviceHandle(), VK_NULL_HANDLE, 1, &createInfo, m_pAllocator->GetAllocator(), &m_VkPipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create compute pipeline!");
	}
}
