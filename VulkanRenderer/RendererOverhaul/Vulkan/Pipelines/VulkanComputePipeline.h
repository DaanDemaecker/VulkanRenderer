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
		/// Add a shader to the list of shaders that this pipeline will use
		/// </summary>
		/// <param name="shaderPath">Absolute or relative path to the shader file</param>
		void AddShader(const std::string& shaderPath);

		/// <summary>
		/// Create the compute pipeline
		/// </summary>
		void CreatePipeline();
	private:
		// Handle of the pipeline object
		VkPipeline m_VkPipeline{ VK_NULL_HANDLE };

		// List of file paths to the shader objects used in this pipeline
		std::vector<std::string> m_ShaderPaths{};
	};
}

#endif // !_DDM_VULKAN_COMPUTE_PIPELINE_
