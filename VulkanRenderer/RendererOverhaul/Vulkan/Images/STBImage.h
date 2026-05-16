// STBImage.h
// A wrapper for an image loaded with the STB_Image library

#ifndef _DDM_STB_IMAGE_
#define _DDM_STB_IMAGE_

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



	private:
		int m_Width{};

		int m_Height{};

		int m_Channels{};

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
