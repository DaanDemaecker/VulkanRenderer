// VulkanPipelineLayout.h
// This class will serve as a wrapper for a Vulkan pipeline layout

#ifndef _DDM_VULKAN_PIPELINE_LAYOUT_
#define _DDM_VULKAN_PIPELINE_LAYOUT_

// Parent include
#include "Vulkan/Core/VulkanObject.h"

namespace DDM::Vulkan
{
	// Class forward declarations
	class VulkanShaderModule;
	class VulkanDescriptorSetLayout;

	class VulkanPipelineLayout final : public VulkanObject
	{
	public:
		// Delete default constructor
		VulkanPipelineLayout() = delete;

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="pCore">Pointer to the Vulkan core</param>
		/// <param name="pAllocator">Pointer to the Vulkan allocator</param>
		/// <param name="shaderModules">Vector of pointers to Shadermodules that make up this layout</param>
		VulkanPipelineLayout(const VulkanCore* pCore, const VulkanAllocator* pAllocator, std::vector<std::unique_ptr<VulkanShaderModule>>& shaderModules);

		/// <summary>
		/// Destructor
		/// </summary>
		virtual ~VulkanPipelineLayout();

		/// <summary>
		/// Get the handle of the pipeline layout
		/// </summary>
		VkPipelineLayout GetPipelineLayout() const;
	private:
		// Handle of the pipeline layout object
		VkPipelineLayout m_VkPipelineLayout{ VK_NULL_HANDLE };

		// List of descriptorsetlayouts used for this layout
		std::vector<std::unique_ptr<VulkanDescriptorSetLayout>> m_DescriptorSetLayouts{};

		/// <summary>
		/// Create the pipeline layout with the given shader modules
		/// </summary>
		/// <param name="shaderModules">Vector of pointers to Shadermodules that make up this layout</param>
		void CreateLayout(std::vector<std::unique_ptr<VulkanShaderModule>>& shaderModules);
	};
}

#endif // !_DDM_VULKAN_PIPELINE_LAYOUT_
