// DDMFileSystem.cpp

// Header include
#include "DDMFileSystem.h"

// Standard library includes
#include <filesystem>

DDM::DDMFileSystem::DDMFileSystem()
{
}

DDM::DDMFileSystem::~DDMFileSystem()
{
	for (auto& file : m_WriteFiles)
	{
		if (file.second.is_open())
		{
			file.second.close();
		}
	}
}

bool DDM::DDMFileSystem::OpenWrite(std::string& fileName)
{
	if (m_ReadFiles.contains(fileName) || (m_WriteFiles.contains(fileName) && m_WriteFiles[fileName].is_open()))
	{
		return false;
	}

	// Get the index of the final period in the name, all characters after it indicate the extension
	auto index = fileName.find_last_of("/");

	auto directoryName = fileName.substr(0, index);

	std::filesystem::create_directory(directoryName);

	m_WriteFiles[fileName] = std::ofstream();
	m_WriteFiles[fileName].open(fileName.c_str(), std::ios::binary | std::ios::out);

	return m_WriteFiles[fileName].is_open();
}

bool DDM::DDMFileSystem::CloseWrite(std::string& fileName)
{
	if (!m_WriteFiles.contains(fileName) || !m_WriteFiles[fileName].is_open())
	{
		return false;
	}

	m_WriteFiles[fileName].close();

	m_WriteFiles.erase(fileName);

	return true;
}
