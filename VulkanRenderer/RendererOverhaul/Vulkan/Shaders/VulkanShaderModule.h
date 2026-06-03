// VulkanShaderModule.h
// This class will serve as a wrapper for a VkShaderModule object

#ifndef _DDM_VULKAN_SHADER_MODULE_
#define _DDM_VULKAN_SHADER_MODULE_

// Header include
#include "Vulkan/Core/VulkanObject.h"

// File includes
#include "Includes/VulkanIncludes.h"

// Standard library includes
#include <string>
#include <vector>

namespace DDM::Vulkan
{
	class VulkanShaderModule final : public VulkanObject
	{
	public:
		// Delete default constructor
		VulkanShaderModule() = delete;

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="pCore">Pointer to the VulkanCore instance</param>
		/// <param name="pAllocator">Pointer to the VulkanAllocator instance</param>
		VulkanShaderModule(const VulkanCore* pCore, const VulkanAllocator* pAllocator);

		/// <summary>
		/// Destructor
		/// </summary>
		virtual ~VulkanShaderModule();

		// Delete copy and move constructors
		VulkanShaderModule(VulkanShaderModule&) = delete;
		VulkanShaderModule(VulkanShaderModule&&) = delete;

		// Delete copy and move assignment operators
		VulkanShaderModule& operator=(VulkanShaderModule&) = delete;
		VulkanShaderModule& operator=(VulkanShaderModule&&) = delete;

		/// <summary>
		/// Create the shader module from the shader code read from the file
		/// </summary>
		/// <param name="filePath">Relative or absolute path to the shader file</param>
		void CreateShaderModule(const std::string& filePath);
	private:
		// Vulkan shader module
		VkShaderModule m_VkShaderModule{ VK_NULL_HANDLE };

		// Buffer to hold the shader code read from the file
		std::vector<char> m_ShaderCode{};

		// Indicates whether the shader code has been read from the file
		bool m_ShaderCodeRead{ false };

		/// <summary>
		/// Read in the shader code from a file and create a VkShaderModule object
		/// </summary>
		/// <param name="filePath">Relative or absolute path to the shader file</param>
		void ReadFromFile(const std::string& filePath);
	};
}

#endif // !_DDM_VULKAN_SHADER_MODULE_
