// VulkanFileImage.h
// A wrapper for a VKImage class that is created from a file

#ifndef _DDM_VULKAN_FILE_IMAGE_
#define _DDM_VULKAN_FILE_IMAGE_

// Parent include
#include "Vulkan/Images/VulkanImage.h"

namespace DDM
{
	// Class forward declarations
	class STBImage;

	class VulkanFileImage final : public VulkanImage
	{
	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="pAllocator">pointer to the custom allocator</param>
		/// <param name="pCore">pointer to VulkanCore object</param>
		VulkanFileImage(const VulkanAllocator* pAllocator, const VulkanCore* pCore, const VulkanCommandPool* pCommandPool);
		
		/// <summary>
		 /// Destructor
		 /// </summary>
		virtual ~VulkanFileImage();

		/// <summary>
		/// Load the image from a file and create the Vulkan image object
		/// </summary>
		/// <param name="filePath">path to the image file</param>
		void LoadImage(const std::string& filePath);
	private:

		/// <summary>
		/// Create the VkImage object
		/// </summary>
		/// <param name="pSTBImage">Pointer to the stb image with data</param>
		void CreateImage(STBImage* pSTBImage);

		/// <summary>
		/// Copy the data from the temp buffer to the image
		/// </summary>
		/// <param name="pBuffer">Pointer to the buffer to copy from</param>
		void CopyBufferToImage(VulkanBuffer* pBuffer);
	};
}

#endif // !_DDM_VULKAN_FILE_IMAGE_