// ConfigManager.h
// This singleton will read, write and manage the config file

#ifndef _DDM_CONFIG_MANAGER_
#define _DDM_CONFIG_MANAGER_

// File includes
#include "Singleton.h"
#include "Includes/RapidJSONIncludes.h"

// Standard library includes
#include <iostream>

namespace DDM
{
	class ConfigManager : public DDM::Singleton<ConfigManager>
	{
	public:
		// Get string object from json
		// Parameters:
		//     propertyName: name of the property
		const char* GetString(const std::string& propertyName);

		// Get string object from json
		// Parameters:
		//     propertyName: name of the property
		const char* GetString(const std::string&& propertyName);

		// Get int object from json
		// Parameters:
		//     propertyName: name of the property
		int GetInt(const std::string& propertyName);

		// Get int object from json
		// Parameters:
		//     propertyName: name of the property
		int GetInt(const std::string&& propertyName);

		// Get bool object from json
		// Parameters:
		//     propertyName: name of the property
		bool GetBool(const std::string& propertyName);

		// Get bool object from json
		// Parameters:
		//     propertyName: name of the property
		bool GetBool(const std::string&& propertyName);

		// Get float object from json
		// Parameters:
		//     propertyName: name of the property
		float GetFloat(const std::string& propertyName);

		// Get float object from json
		// Parameters:
		//     propertyName: name of the property
		float GetFloat(const std::string&& propertyName);

	private:
		friend class Singleton<ConfigManager>;
		ConfigManager();

		// File name of the config file
		const std::string m_FileName{ "Config.json" };

		// The default string when none is found
		const std::string m_DefaultString{ "Not availabe" };

		// The json file object
		rapidjson::Document m_JsonFile{};
	};
}

#endif // !_DDM_CONFIG_MANAGER_