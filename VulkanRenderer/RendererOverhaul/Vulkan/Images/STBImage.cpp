// STBImage.cpp

// Header include
#include "STBImage.h"

// File includes
#include "Includes/STBIncludes.h"

// Standard library includes
#include <iostream>

DDM::STBImage::STBImage(const std::string& path)
{
	LoadImage(path);
}

DDM::STBImage::~STBImage()
{
	// Only free the image if it was correctly initialized
	if (m_Initialized)
	{
		stbi_image_free(m_pData);
	}
}

void DDM::STBImage::LoadImage(const std::string& path)
{
	m_pData = stbi_load(path.c_str(), &m_Width, &m_Height, &m_Channels, 4);

	if (m_pData != nullptr)
	{
		m_Initialized = true;
	}
#ifndef NDEBUG
	else
	{
		std::cout << "Failed to open file: " << path << " Reason: " << stbi_failure_reason() << std::endl;
	}
#endif // !NDEBUG	
}
