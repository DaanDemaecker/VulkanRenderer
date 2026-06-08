// VulkanComputePipeline.h
// This class will serve as a wrapper for a Vulkan compute pipeline

#ifndef _DDM_VULKAN_COMPUTE_PIPELINE_
#define _DDM_VULKAN_COMPUTE_PIPELINE_

// Parent include
#include "Vulkan/Core/VulkanObject.h"

// File includes
#include "Includes/VulkanIncludes.h"

// Standard library includes
#include <vector>
#include <string>

namespace DDM::Vulkan
{
	// Class forward declarations
	class VulkanShaderModule;
	class VulkanSpecInfo;

	class VulkanComputePipeline final : public VulkanObject
	{
	public:
		// Delete default constructor
		VulkanComputePipeline() = delete;

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="pCore">Pointer to the VulkanCore instance</param>
		/// <param name="pAllocator">Pointer to the VulkanAllocator instance</param>
		VulkanComputePipeline(const VulkanCore* pCore, const VulkanAllocator* pAllocator);

		/// <summary>
		/// Destructor
		/// </summary>
		virtual ~VulkanComputePipeline();

		/// <summary>
		/// Set the path to the shader file that this pipeline will use
		/// </summary>
		/// <param name="shaderPath">Absolute or relative path to the shader file</param>
		void AddShader(const std::string& shaderPath);

		/// <summary>
		/// Create the compute pipeline
		/// </summary>
		void CreatePipeline();

		/// <summary>
		/// Set the specialization info for this pipeline
		/// </summary>
		/// <param name="specInfo">Unique pointer to specinfo object</param>
		void SetSpecInfo(std::unique_ptr<VulkanSpecInfo> specInfo);
	private:
		// Handle of the pipeline object
		VkPipeline m_VkPipeline{ VK_NULL_HANDLE };

		// File path to the shader file used in this pipeline
		std::string m_ShaderPath{};

		// Pointer to the 
		std::unique_ptr<VulkanSpecInfo> m_pSpecializationInfo{};

		/// <summary>
		/// Set up the embedded shader stage create info struct
		/// </summary>
		/// <param name="shaderModule">Pointer to the shader module to read the stage info from</param>
		/// <param name="stageInfo">reference to the struct to fill in</param>
		void SetupStageInfo(VulkanShaderModule* shaderModule, VkPipelineShaderStageCreateInfo& stageInfo);
	};
}

#endif // !_DDM_VULKAN_COMPUTE_PIPELINE_
