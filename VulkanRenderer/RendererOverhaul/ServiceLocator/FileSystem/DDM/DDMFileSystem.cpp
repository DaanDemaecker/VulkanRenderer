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

bool DDM::DDMFileSystem::Write(std::string& fileName, const char* start, size_t size)
{
	if (!m_WriteFiles.contains(fileName) || !m_WriteFiles[fileName].is_open())
	{
		if (!OpenWrite(fileName))
		{
			return false;
		}
	}

	m_WriteFiles[fileName].write(start, size);

	return true;
}

bool DDM::DDMFileSystem::OpenRead(std::string& fileName)
{
	if (m_WriteFiles.contains(fileName) || (m_ReadFiles.contains(fileName) && m_ReadFiles[fileName].is_open()))
	{
		return false;
	}

	if (!std::filesystem::exists(fileName))
	{
		return false;
	}

	m_ReadFiles[fileName] = std::ifstream();
	m_ReadFiles[fileName].open(fileName, std::ios::binary);

	return true;
}

bool DDM::DDMFileSystem::CloseRead(std::string& fileName)
{
	if (!m_ReadFiles.contains(fileName) || !m_ReadFiles[fileName].is_open())
	{
		return false;
	}

	m_ReadFiles[fileName].close();

	m_ReadFiles.erase(fileName);

	return true;
}

bool DDM::DDMFileSystem::Read(std::string& fileName, char* start, size_t size)
{
	if (!m_ReadFiles.contains(fileName) || !m_ReadFiles[fileName].is_open())
	{
		if (!OpenRead(fileName))
		{
			return false;
		}
	}

	m_ReadFiles[fileName].read(start, size);

	return true;
}
