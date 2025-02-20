// ConfigManager.h
// This singleton will read, write and manage the config file

#ifndef ConfigManagerIncluded
#define ConfigManagerIncluded

// Parent class include
#include "Singleton.h"

// File includes
#include "Includes/RapidJSONIncludes.h"

// Standard library includes
#include <string>


namespace DDM3
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
		// File name of the config file
		const std::string m_FileName { "Config.json" };

		// The default string when none is found
		const std::string m_DefaultString{ "Not availabe" };

		// The json file object
		rapidjson::Document m_JsonFile{};

		void ReadFile();
	};
}

#endif // !ConfigManagerIncluded
