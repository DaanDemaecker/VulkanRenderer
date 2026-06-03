// VulkanShaderModule.cpp

// Header include
#include "VulkanShaderModule.h"

// File includes
#include "Vulkan/Core/VulkanCore.h"
#include "Vulkan/Core/VulkanAllocator.h"

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
