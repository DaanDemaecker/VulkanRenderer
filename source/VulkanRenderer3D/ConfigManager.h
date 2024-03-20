// ConfigManager.h
// This singleton will read, write and manage the config file

// File includes
#include "Singleton.h"
#include "RapidJSONIncludes.h"

// Standard library includes
#include <iostream>

namespace D3D
{
	class ConfigManager : public Singleton<ConfigManager>
	{
	public:
		// Constructor
		ConfigManager();


		// Get string object from json
		// Parameters:
		//     propertyName: name of the property
		std::string GetString(const std::string& propertyName);

		// Get string object from json
		// Parameters:
		//     propertyName: name of the property
		std::string GetString(const std::string&& propertyName);

	private:
		const std::string m_FileName { "Config.json" };

		const std::string m_DefaultString{ "Not availabe" };

		rapidjson::Document m_JsonFile{};
	};
}

