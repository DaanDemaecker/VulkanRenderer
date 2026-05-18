// STBImage.h
// A wrapper for an image loaded with the STB_Image library

#ifndef _DDM_STB_IMAGE_
#define _DDM_STB_IMAGE_

// File includes
#include "Includes/VulkanIncludes.h"

// Standard library includes
#include <string>

namespace DDM
{
	class STBImage final
	{
	public:
		// Delte default constructor
		STBImage() = delete;

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="path">relative or absolute filepath to the image to load</param>
		STBImage(const std::string& path);

		// Delete copy and move constructors
		STBImage(STBImage&) = delete;
		STBImage(STBImage&&) = delete;

		// Delete copy and move asignment operators
		STBImage& operator=(STBImage&) = delete;
		STBImage& operator=(STBImage&&) = delete;

		/// <summary>
		/// Destructor
		/// </summary>
		~STBImage();

		/// <summary>
		/// Get the Vulkan extent of the loaded image
		/// </summary>
		/// <returns>VKExtent3D structure holding values of the image size</returns>
		VkExtent3D GetExtent() const;

		/// <summary>
		/// Get the size of the image in bytes
		/// </summary>
		/// <returns>Size of image in bytes</returns>
		VkDeviceSize GetSize() const;

		/// <summary>
		/// Get a pointer to the image data
		/// </summary>
		/// <returns>Pointer to the image data</returns>
		void* GetHandle() const;
	private:
		int m_Width{};

		int m_Height{};

		int m_Channels{};

		int m_DesiredChannels{ 4 };

		void* m_pData{};

		bool m_Initialized{};


		/// <summary>
		/// Load in the image at the given filepath
		/// </summary>
		/// <param name="path">relative or absolute filepath to the image to load</param>
		void LoadImage(const std::string& path);
	};
}

#endif // !_DDM_STB_IMAGE_
