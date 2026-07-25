// ConfigManager.cpp

// Header include
#include "ConfigManager.h"

// File includes
#include "Includes/RapidJsonIncludes.h"

// Standard library includes
#include <vector>
#include <stdexcept>
#include <iostream>

namespace DDM
{
	class ConfigManagerImpl
	{
	public:
		// Delete default constructor
		ConfigManagerImpl() = delete;

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="fileName">Name/path of the config file</param>
		ConfigManagerImpl(const std::string& fileName)
		{
			FILE* pFile{};

			// Open config file in read mode
			auto result{ fopen_s(&pFile, fileName.c_str(), "r") };

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

		/// <summary>
		/// Retrieve a string property from the config file
		/// </summary>
		/// <param name="propertyName">Name of the property</param>
		/// <returns>C-String holding property</returns>
		const char* GetString(const std::string& propertyName)
		{
			// Check if file contains property, if not, return default string
			if (m_JsonFile.HasMember(propertyName.c_str()) && m_JsonFile[propertyName.c_str()].IsString())
			{
				return m_JsonFile[propertyName.c_str()].GetString();
			}

			std::cout << "String property " << propertyName << " is not availabel\n";
			return m_DefaultString.c_str();
		}

		/// <summary>
		/// Retrieve an int property from the config file
		/// </summary>
		/// <param name="propertyName">Name of the property</param>
		/// <returns>Int holding property</returns>
		int GetInt(const std::string& propertyName)
		{
			// Check if file contains property, if not, return 0
			if (m_JsonFile.HasMember(propertyName.c_str()) && m_JsonFile[propertyName.c_str()].IsInt())
			{
				return m_JsonFile[propertyName.c_str()].GetInt();
			}

			std::cout << "Int property " << propertyName << " is not availabel\n";

			return 0;
		}

		/// <summary>
		/// Retrieve a bool property from the config file
		/// </summary>
		/// <param name="propertyName">Name of the property</param>
		/// <returns>Bool holding property</returns>
		bool GetBool(const std::string& propertyName)
		{
			// Check if file contains property, if not, return false
			if (m_JsonFile.HasMember(propertyName.c_str()) && m_JsonFile[propertyName.c_str()].IsBool())
			{
				return m_JsonFile[propertyName.c_str()].GetBool();
			}

			std::cout << "Bool property " << propertyName << " is not availabel\n";
			return false;
		}

		/// <summary>
		/// Retrieve a float property from the config file
		/// </summary>
		/// <param name="propertyName">Name of the property</param>
		/// <returns>Float holding property</returns>
		float GetFloat(const std::string& propertyName)
		{
			// Check if file contains property, if not, return 0
			if (m_JsonFile.HasMember(propertyName.c_str()) && m_JsonFile[propertyName.c_str()].IsFloat())
			{
				return m_JsonFile[propertyName.c_str()].GetFloat();
			}

			std::cout << "Float property " << propertyName << " is not availabel\n";
			return 0;
		}

	private:
		// The default string when none is found
		const std::string m_DefaultString{ "Not availabe" };

		// The json file object
		rapidjson::Document m_JsonFile{};
	};
}



DDM::ConfigManager::~ConfigManager()
{

}

DDM::ConfigManager::ConfigManager()
{
	m_pImpl = std::make_unique<ConfigManagerImpl>(m_FileName);
}

const char* DDM::ConfigManager::GetString(const std::string& propertyName)
{
	return m_pImpl->GetString(propertyName);
}

int DDM::ConfigManager::GetInt(const std::string& propertyName)
{
	return m_pImpl->GetInt(propertyName);
}

bool DDM::ConfigManager::GetBool(const std::string& propertyName)
{
	return m_pImpl->GetBool(propertyName);
}

float DDM::ConfigManager::GetFloat(const std::string& propertyName)
{
	return m_pImpl->GetFloat(propertyName);
}

