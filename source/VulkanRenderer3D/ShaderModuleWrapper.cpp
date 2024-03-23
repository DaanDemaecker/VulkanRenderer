// ShaderModuleWrapper.cpp

// File includes
#include "ShaderModuleWrapper.h"
#include "Utils.h"

// Standard library includes
#include <stdexcept>

D3D::ShaderModuleWrapper::ShaderModuleWrapper(VkDevice device, const std::string& filePath)
{
	m_ShaderCode = Utils::readFile(filePath);

	CreateShaderModule(device);

	ReflectShader();

	CreateShaderStageInfo();

	
}

void D3D::ShaderModuleWrapper::Cleanup(VkDevice device)
{
	spvReflectDestroyShaderModule(&m_ReflectShaderModule);
	// Delete fragment shader module
	vkDestroyShaderModule(device, m_ShaderModule, nullptr);
}

void D3D::ShaderModuleWrapper::CreateShaderModule(VkDevice device)
{
	// Create modlue create info
	VkShaderModuleCreateInfo createInfo{};
	// Set type to shader module craete info
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	// Set the code size to the size of the vector
	createInfo.codeSize = m_ShaderCode.size();
	// Give the data of the vector as a pointer
	createInfo.pCode = reinterpret_cast<const uint32_t*>(m_ShaderCode.data());

	// Create shadermodule
	if (vkCreateShaderModule(device, &createInfo, nullptr, &m_ShaderModule) != VK_SUCCESS)
	{
		// Unsuccessful, throw runtime error
		throw std::runtime_error("failed to create shader module!");
	}
}

void D3D::ShaderModuleWrapper::ReflectShader()
{
	SpvReflectResult result = spvReflectCreateShaderModule(m_ShaderCode.size() * sizeof(char), m_ShaderCode.data(), &m_ReflectShaderModule);

	if (result != SPV_REFLECT_RESULT_SUCCESS)
	{
		throw std::runtime_error("failed to create shader module");
	}
}

void D3D::ShaderModuleWrapper::CreateShaderStageInfo()
{
	// Set type to pipeline shader stage create info
	m_ShaderstageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	// Set stage to fragment
	m_ShaderstageCreateInfo.stage =
		static_cast<VkShaderStageFlagBits>(m_ReflectShaderModule.shader_stage);
	// Give the correct shader module
	m_ShaderstageCreateInfo.module = m_ShaderModule;
	// Give the name of the function
	m_ShaderstageCreateInfo.pName = m_ReflectShaderModule.entry_point_name;
}
