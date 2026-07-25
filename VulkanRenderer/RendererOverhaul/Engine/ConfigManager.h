// ConfigManager.h
// This singleton will read the config file

#ifndef _DDM_CONFIG_MANAGER_
#define _DDM_CONFIG_MANAGER_

// Parent include
#include "Singleton.h"

// Standard library includes
#include <string>
#include <memory>

namespace DDM
{
	// Class forward declarations
	class ConfigManagerImpl;

	class ConfigManager final : public Singleton<ConfigManager>
	{
	public:
		// Delete copy and move operations
		ConfigManager(ConfigManager&) = delete;
		ConfigManager(ConfigManager&&) = delete;

		ConfigManager& operator=(ConfigManager&) = delete;
		ConfigManager& operator=(ConfigManager&&) = delete;

		// Destructor
		~ConfigManager();

		/// <summary>
		/// Retrieve a string property from the config file
		/// </summary>
		/// <param name="propertyName">Name of the property</param>
		/// <returns>C-String holding property</returns>
		const char* GetString(const std::string& propertyName);

		/// <summary>
		/// Retrieve an int property from the config file
		/// </summary>
		/// <param name="propertyName">Name of the property</param>
		/// <returns>Int holding property</returns>
		int GetInt(const std::string& propertyName);

		/// <summary>
		/// Retrieve a bool property from the config file
		/// </summary>
		/// <param name="propertyName">Name of the property</param>
		/// <returns>Bool holding property</returns>
		bool GetBool(const std::string& propertyName);

		/// <summary>
		/// Retrieve a float property from the config file
		/// </summary>
		/// <param name="propertyName">Name of the property</param>
		/// <returns>Float holding property</returns>
		float GetFloat(const std::string& propertyName);

	private:
		// Private constructor to avoid multiple instances of singleton
		friend class Singleton<ConfigManager>;
		ConfigManager();

		std::unique_ptr<ConfigManagerImpl> m_pImpl{};

		std::string m_FileName{ "Config.json" };
	};
}

#endif // !_DDM_CONFIG_MANAGER_