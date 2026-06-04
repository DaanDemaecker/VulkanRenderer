// VulkanPipelineLayout.h
// This class will serve as a wrapper for a Vulkan pipeline layout

#ifndef _DDM_VULKAN_PIPELINE_LAYOUT_
#define _DDM_VULKAN_PIPELINE_LAYOUT_

// Parent include
#include "Vulkan/Core/VulkanObject.h"

namespace DDM::Vulkan
{
	class VulkanPipelineLayout final : public VulkanObject
	{
	public:
		// Delete default constructor
		VulkanPipelineLayout() = delete;
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="device">Handle to the logical device</param>
		VulkanPipelineLayout(VkDevice device);
		/// <summary>
		/// Destructor
		/// </summary>
		virtual ~VulkanPipelineLayout();
		/// <summary>
		/// Get the handle of the pipeline layout
		/// </summary>
		VkPipelineLayout GetPipelineLayout() const { return m_VkPipelineLayout; }
	private:
		// Handle of the pipeline layout object
		VkPipelineLayout m_VkPipelineLayout{ VK_NULL_HANDLE };
	};
}

#endif // !_DDM_VULKAN_PIPELINE_LAYOUT_
