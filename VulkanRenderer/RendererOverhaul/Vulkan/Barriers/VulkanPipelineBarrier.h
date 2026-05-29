// VulkanPipelineBarrier.h
// A helper class to simplify the use of pipeline barriers

#ifndef _DDM_VULKAN_PIPELINE_BARRIER_
#define _DDM_VULKAN_PIPELINE_BARRIER_

// File includes
#include "Includes/VulkanIncludes.h"

// Standard library includes
#include <vector>
#include <memory>

namespace DDM::Vulkan
{
	// Class forward declarations
	class VulkanImageBarrier;

	class VulkanPipelineBarrier final
	{
	public:
		// Delete default constructor
		VulkanPipelineBarrier() = delete;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="srcStageMask"></param>
		/// <param name="dstStageMask"></param>
		/// <param name="flags"></param>
		VulkanPipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags flags);

		// Delete copy and move constructors
		VulkanPipelineBarrier(VulkanPipelineBarrier&) = delete;
		VulkanPipelineBarrier(VulkanPipelineBarrier&&) = delete;

		// Delete copy and move asignment operators
		VulkanPipelineBarrier& operator=(VulkanPipelineBarrier&) = delete;
		VulkanPipelineBarrier& operator=(VulkanPipelineBarrier&&) = delete;

		/// <summary>
		/// Destructor
		/// </summary>
		~VulkanPipelineBarrier();

		/// <summary>
		/// Execute the pipeline barrier
		/// </summary>
		/// <param name="commandBuffer">Commandbuffer to execute on</param>
		void Execute(VkCommandBuffer commandBuffer);

		/// <summary>
		/// Add an image barrier
		/// </summary>
		/// <param name="pImageBarrier">Image barrier to add</param>
		void AddImageBarriers(std::unique_ptr<VulkanImageBarrier> pImageBarrier);
	private:
		// Source stage mask
		const VkPipelineStageFlags m_SrcStageMask;

		// Destination stage mask
		const VkPipelineStageFlags m_DstStageMask;

		// Dependency flags
		const VkDependencyFlags m_DependencyFlags;

		std::vector<std::unique_ptr<VulkanImageBarrier>> m_ImageBarriers{};
	};
}

#endif // !_DDM_VULKAN_PIPELINE_BARRIER_
