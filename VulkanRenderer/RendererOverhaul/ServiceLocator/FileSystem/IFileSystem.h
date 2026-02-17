// IFileSystem.h
// This file will hold the baseclass for filesystems
// FIlesystems will be used to write to and read files

#ifndef _DDM_I_FILE_SYSTEM_
#define _DDM_I_FILE_SYSTEM_

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
	};
}

#endif // !_DDM_I_FILE_SYSTEM_