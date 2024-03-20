// ConfigManager.cpp

// File includes
#include "ConfigManager.h"

// Standard library includes
#include <iostream>
#include <stdexcept>
#include <vector>

D3D::ConfigManager::ConfigManager()
{
	FILE* pFile{};

	// Open config file in read mode
	auto result{ fopen_s(&pFile, m_FileName.c_str(), "r")};

	// If not successful, throw runtime error
	if (result != 0)
	{
		throw std::runtime_error("failed to open config file");
	}

	// Determine size of file by moving location indicator to end of file and reading the position
	fseek(pFile, 0, SEEK_END);
	size_t fileSize = ftell(pFile);
	// Move location indicator back to beginning of file
	fseek(pFile, 0, SEEK_SET);

	// Create readbuffer the size of file and stream to FileReadStream object
	std::vector<char> readBuffer(fileSize);
	rapidjson::FileReadStream is(pFile, readBuffer.data(), fileSize);

	// Parse the JSON data  
	m_JsonFile.ParseStream(is);

	// Close the file
	fclose(pFile);
}

std::string D3D::ConfigManager::GetString(const std::string& propertyName)
{
	// Check if file contains property, if not, return default string
	if (m_JsonFile.HasMember(propertyName.c_str()) && m_JsonFile[propertyName.c_str()].IsString())
	{
		return m_JsonFile[propertyName.c_str()].GetString();
	}

	return m_DefaultString;
}

std::string D3D::ConfigManager::GetString(const std::string&& propertyName)
{
	return GetString(propertyName);
}

int D3D::ConfigManager::GetInt(const std::string& propertyName)
{

	// Check if file contains property, if not, return 0
	if (m_JsonFile.HasMember(propertyName.c_str()) && m_JsonFile[propertyName.c_str()].IsInt())
	{
		return m_JsonFile[propertyName.c_str()].GetInt();
	}
	return 0;
}

int D3D::ConfigManager::GetInt(const std::string&& propertyName)
{
	return GetInt(propertyName);
}
