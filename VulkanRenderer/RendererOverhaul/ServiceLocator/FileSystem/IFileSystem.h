// IFileSystem.h
// This file will hold the baseclass for filesystems
// FIlesystems will be used to write to and read files

#ifndef _DDM_I_FILE_SYSTEM_
#define _DDM_I_FILE_SYSTEM_

// File includes
#include <string>

namespace DDM
{
	class IFileSystem
	{
	public:
		IFileSystem() = default;
		virtual ~IFileSystem() = default;

		IFileSystem(IFileSystem&) = delete;
		IFileSystem(IFileSystem&&) = delete;

		IFileSystem& operator=(IFileSystem&) = delete;
		IFileSystem& operator=(IFileSystem&&) = delete;

		virtual bool OpenWrite(std::string& fileName) = 0;

		virtual bool CloseWrite(std::string& fileName) = 0;

		virtual bool Write(std::string& fileName, const char* start, size_t size) = 0;

		virtual bool OpenRead(std::string& fileName) = 0;

		virtual bool CloseRead(std::string& fileName) = 0;

		virtual bool Read(std::string& fileName, char* start, size_t size) = 0;
	};

	class DefaultFileSystem final : public IFileSystem
	{
	public:
		DefaultFileSystem() = default;
		virtual ~DefaultFileSystem() = default;

		DefaultFileSystem(DefaultFileSystem&) = delete;
		DefaultFileSystem(DefaultFileSystem&&) = delete;

		DefaultFileSystem& operator=(DefaultFileSystem&) = delete;
		DefaultFileSystem& operator=(DefaultFileSystem&&) = delete;

		virtual bool OpenWrite(std::string& /*filename*/) override { return false; }

		virtual bool CloseWrite(std::string& /*fileName*/) override { return false; }

		virtual bool Write(std::string& /*fileName*/, const char* /*start*/, size_t /*size*/) override { return false; }

		virtual bool OpenRead(std::string& /*fileName*/) override { return false; }

		virtual bool CloseRead(std::string& /*fileName*/) override { return false; }

		virtual bool Read(std::string& /*fileName*/, char* /*start*/, size_t /*size*/) override { return false; }
	};
}

#endif // !_DDM_I_FILE_SYSTEM_