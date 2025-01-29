// ConfigManager.cpp

// Header include
#include "ConfigManager.h"

// Standard library includes
#include <iostream>
#include <stdexcept>
#include <vector>

DDM3::ConfigManager::ConfigManager()
{
	// Read the config file
	ReadFile();
}

std::string DDM3::ConfigManager::GetString(const std::string& propertyName)
{
	// Check if file contains property, if not, return default string
	if (m_JsonFile.HasMember(propertyName.c_str()) && m_JsonFile[propertyName.c_str()].IsString())
	{
		return m_JsonFile[propertyName.c_str()].GetString();
	}

	std::cout << "String property " << propertyName << " is not availabel\n";
	return m_DefaultString;
}

std::string DDM3::ConfigManager::GetString(const std::string&& propertyName)
{
	return GetString(propertyName);
}

int DDM3::ConfigManager::GetInt(const std::string& propertyName)
{

	// Check if file contains property, if not, return 0
	if (m_JsonFile.HasMember(propertyName.c_str()) && m_JsonFile[propertyName.c_str()].IsInt())
	{
		return m_JsonFile[propertyName.c_str()].GetInt();
	}

	std::cout << "Int property " << propertyName << " is not availabel\n";

	return 0;
}

int DDM3::ConfigManager::GetInt(const std::string&& propertyName)
{
	return GetInt(propertyName);
}

bool DDM3::ConfigManager::GetBool(const std::string& propertyName)
{
	// Check if file contains property, if not, return false
	if (m_JsonFile.HasMember(propertyName.c_str()) && m_JsonFile[propertyName.c_str()].IsBool())
	{
		return m_JsonFile[propertyName.c_str()].GetBool();
	}

	std::cout << "Bool property " << propertyName << " is not availabel\n";
	return false;
}

bool DDM3::ConfigManager::GetBool(const std::string&& propertyName)
{
	return GetBool(propertyName);
}

float DDM3::ConfigManager::GetFloat(const std::string& propertyName)
{
	// Check if file contains property, if not, return 0
	if (m_JsonFile.HasMember(propertyName.c_str()) && m_JsonFile[propertyName.c_str()].IsFloat())
	{
		return m_JsonFile[propertyName.c_str()].GetFloat();
	}

	std::cout << "Float property " << propertyName << " is not availabel\n";
	return 0;
}

float DDM3::ConfigManager::GetFloat(const std::string&& propertyName)
{
	return GetFloat(propertyName);
}

void DDM3::ConfigManager::ReadFile()
{
	FILE* pFile{};

	// Open config file in read mode
	auto result{ fopen_s(&pFile, m_FileName.c_str(), "r") };

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
