// CommandBuffer.h
// This class will serve as a wrapper for a Vulkan CommanBuffer

#ifndef _DDM_VULKAN_COMMAND_BUFFER_
#define _DDM_VULKAN_COMMAND_BUFFER_

// File includes
#include "Includes/VulkanIncludes.h"

namespace DDM::Vulkan
{
	// Class forward declarations
	class VulkanCore;
	class VulkanCommandPool;
	class VulkanQueue;
	class VulkanPipelineBarrier;
	class VulkanBuffer;
	class VulkanImage;

	class VulkanCommandBuffer final
	{
	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="pPool">Owning commandpool</param>
		/// <param name="pQueue">Queue to execute on</param>
		/// <param name="resetBitSet">Indicates whether commandbuffer can be reset</param>
		VulkanCommandBuffer(const VulkanCommandPool* pPool, const VulkanQueue* pQueue, const bool resetBitSet);

		// Destructor
		~VulkanCommandBuffer();

		// Delete default constructor
		VulkanCommandBuffer() = delete;

		// Delete copy and move constructors
		VulkanCommandBuffer(VulkanCommandBuffer&) = delete;
		VulkanCommandBuffer(VulkanCommandBuffer&&) = delete;

		// Delete copy and move assignment operators
		VulkanCommandBuffer& operator=(VulkanCommandBuffer&) = delete;
		VulkanCommandBuffer& operator=(VulkanCommandBuffer&&) = delete;

		/// <summary>
		/// Submit the command buffer
		/// </summary>
		void Submit();

		// ------------------------------------------------------------------------------
		// Commands
		// ------------------------------------------------------------------------------

		/// <summary>
		/// Record a pipeline barrier to the commandbuffer
		/// </summary>
		/// <param name="pBarrier">Pointer to the pipeline barrier object</param>
		void CmdPipelineBarrier(VulkanPipelineBarrier* pBarrier);

		/// <summary>
		/// Copy a vulkan buffer to a vulkan image
		/// </summary>
		/// <param name="pImage">Pointer to the image object</param>
		/// <param name="pBuffer">Pointer to the buffer object</param>
		void CmdCopyBufferToImage(VulkanImage* pImage, VulkanBuffer* pBuffer);

		/// <summary>
		/// Copy a vulkan buffer to a vulkan image
		/// </summary>
		/// <param name="pImage">Pointer to the image object</param>
		/// <param name="pBuffer">Pointer to the buffer object</param>
		/// <param name="region">Region info for the copy command</param>
		void CmdCopyBufferToImage(VulkanImage* pImage, VulkanBuffer* pBuffer, VkBufferImageCopy& region);

		/// <summary>
		/// Copy a vulkan image to another vulkan image
		/// </summary>
		/// <param name="pSrcImage">Image to copy from</param>
		/// <param name="pDstImage">Image to copy to</param>
		void CmdCopyImageToImage(VulkanImage* pSrcImage, VulkanImage* pDstImage);

		/// <summary>
		/// Copy a vulkan image to another vulkan image
		/// </summary>
		/// <param name="pSrcImage">Image to copy from</param>
		/// <param name="pDstImage">Image to copy to</param>
		/// <param name="region">Region info for the copy command</param>
		void CmdCopyImageToImage(VulkanImage* pSrcImage, VulkanImage* pDstImage, VkImageCopy& region);
	private:
		// The owning commandpool
		const VulkanCommandPool* m_pPool;

		// The queue to submit to
		const VulkanQueue* m_pQueue;

		// Handle of the vulkan command buffer
		VkCommandBuffer m_VkCommandBuffer{};

		// Indicates whether commandbuffer can be reset
		const bool m_ResetBitSet;
		
		// Indicates whether commandbuffer has been set to record
		bool m_InUse{ false };

		// Indicates whether any commands have been recorded
		bool m_CommandRecorded{ false };

		// Indicates whether queue has been submitted
		bool m_Submitted{ false };

		// Indicates whether queue is unusable
		// This happens when the queue is submitted and the reset bit is not set
		bool m_Unuseable{ false };

		/// <summary>
		/// Make the commandbuffer ready for recording
		/// </summary>
		void BeginCommandBuffer();

		/// <summary>
		/// End recordding of commandbuffer
		/// </summary>
		void EndCommandBuffer();

		/// <summary>
		/// Reset the commandbuffer
		/// </summary>
		void ResetCommandBuffer();

		/// <summary>
		/// Query whether commandbuffer is ready to record
		/// </summary>
		/// <returns>Bool indicating readiness to record</returns>
		bool CanRecord();
	};
}

#endif // !_DDM_VULKAN_COMMAND_BUFFER_
