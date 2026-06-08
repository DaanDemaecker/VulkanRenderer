// VulkanShaderModule.h
// This class will serve as a wrapper for a VkShaderModule object

#ifndef _DDM_VULKAN_SHADER_MODULE_
#define _DDM_VULKAN_SHADER_MODULE_

// Header include
#include "Vulkan/Core/VulkanObject.h"

// File includes
#include "Includes/VulkanIncludes.h"
#include "Vulkan/Shaders/SpirVReflect/spirv_reflect.h"

// Standard library includes
#include <string>
#include <vector>


namespace DDM::Vulkan
{
	// Class forward declarations
	class VulkanDescriptorSetLayout;

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

		/// <summary>
		/// Read descriptorsetlayouts from the reflected shader module and add them to the given vector of descriptor set layouts
		/// </summary>
		/// <param name="descriptorSetLayouts">Vector to store the descriptor set layouts</param>
		void AddDescriptorSetLayout(std::vector<std::unique_ptr<VulkanDescriptorSetLayout>>& descriptorSetLayouts);

		/// <summary>
		/// Read the push constant ranges from the reflected shader module and add them to the given vector of push constant ranges
		/// </summary>
		/// <param name="pushConstantRanges">Vector to store the push constant ranges</param>
		void AddPushConstantRanges(std::vector<VkPushConstantRange>& pushConstantRanges);

		/// <summary>
		/// Set up the given shader stage create info struct with the correct values read from the reflected shader module
		/// </summary>
		/// <param name="stageInfo">Reference to the shader stage create info struct to set up</param>
		void SetupShaderStageInfo(VkPipelineShaderStageCreateInfo& stageInfo);

		/// <summary>
		/// Get a const reference to the reflected shader module
		/// </summary>
		/// <returns>Const reference to the reflected shader module</returns>
		const SpvReflectShaderModule& GetReflectedModule() const { return m_ReflectedModule; }
	private:
		// Vulkan shader module
		VkShaderModule m_VkShaderModule{ VK_NULL_HANDLE };

		// Buffer to hold the shader code read from the file
		std::vector<char> m_ShaderCode{};

		// Indicates whether the shader code has been read from the file
		bool m_ShaderCodeRead{ false };

		// The reflected shader module from the SpirV-Reflect library
		SpvReflectShaderModule m_ReflectedModule{};

		// Indicates whether the shader has been reflected
		bool m_Reflected{ false };

		/// <summary>
		/// Read in the shader code from a file and create a VkShaderModule object
		/// </summary>
		/// <param name="filePath">Relative or absolute path to the shader file</param>
		void ReadFromFile(const std::string& filePath);

		/// <summary>
		/// Reflect the shader
		/// </summary>
		void Reflect();
	};
}

#endif // !_DDM_VULKAN_SHADER_MODULE_
