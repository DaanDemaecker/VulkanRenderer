// IFileSystem.h
// This file will hold the baseclass for filesystems
// FIlesystems will be used to write to and read files

#ifndef _DDM_I_FILE_SYSTEM_
#define _DDM_I_FILE_SYSTEM_

// File includes
#include <string>
#include <vector>
#include <sstream>

namespace DDM::Filesystem
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

		virtual bool OpenWrite(const std::string& fileName) = 0;

		virtual bool CloseWrite(const std::string& fileName) = 0;

		virtual bool Write(const std::string& fileName, const char* start, size_t size) = 0;

		virtual bool OpenRead(const std::string& fileName) = 0;

		virtual bool CloseRead(const std::string& fileName) = 0;

		virtual bool Read(const std::string& fileName, char* start, size_t size) = 0;

		virtual bool ReadAll(const std::string& fileName, std::vector<char>& buffer) = 0;
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

		virtual bool OpenWrite(const std::string& /*filename*/) override { return false; }

		virtual bool CloseWrite(const std::string& /*fileName*/) override { return false; }

		virtual bool Write(const std::string& /*fileName*/, const char* /*start*/, size_t /*size*/) override { return false; }

		virtual bool OpenRead(const std::string& /*fileName*/) override { return false; }

		virtual bool CloseRead(const std::string& /*fileName*/) override { return false; }

		virtual bool Read(const std::string& /*fileName*/, char* /*start*/, size_t /*size*/) override { return false; }

		virtual bool ReadAll(const std::string& /*fileName*/, std::vector<char>& /*buffer*/) override { return false; }
	};
}

#endif // !_DDM_I_FILE_SYSTEM_