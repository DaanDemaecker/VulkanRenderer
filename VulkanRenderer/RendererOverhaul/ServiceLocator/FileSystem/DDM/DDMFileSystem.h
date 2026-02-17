// DDMFileSystem.h
// This file will hold an implementation of the IFileSystem

#ifndef _DDM_DDM_FILE_SYSTEM_
#define _DDM_DDM_FILE_SYSTEM_

// Parent include
#include "ServiceLocator/FileSystem/IFileSystem.h"

// Standard library includes
#include <fstream>
#include <unordered_map>

namespace DDM
{
	class DDMFileSystem final : public IFileSystem
	{
	public:
		DDMFileSystem();
		virtual ~DDMFileSystem();

		DDMFileSystem(DDMFileSystem&) = delete;
		DDMFileSystem(DDMFileSystem&&) = delete;

		DDMFileSystem& operator=(DDMFileSystem&) = delete;
		DDMFileSystem& operator=(DDMFileSystem&&) = delete;

		virtual bool OpenWrite(std::string& fileName) override;

		virtual bool CloseWrite(std::string& fileName) override;
	private:
		std::unordered_map<std::string, std::ofstream> m_WriteFiles{};

		std::unordered_map<std::string, std::ifstream> m_ReadFiles{};
	};
}

#endif // !_DDM_DDM_FILE_SYSTEM_
