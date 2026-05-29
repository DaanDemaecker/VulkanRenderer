// ServiceLocator.h

#ifndef _DDM_SERVICE_LOCATOR_
#define _DDM_SERVICE_LOCATOR_

// File includes
#include "FileSystem/IFileSystem.h"
#include "Renderer/IRenderer.h"

// Standard library includes
#include <memory>

namespace DDM
{
	class ServiceLocator final
	{
	public:
		// This class should never be instantiated, so the constructor is deleted
		ServiceLocator() = delete;

		// The destructor is also deleted
		~ServiceLocator() = delete;

		// The copy and move constructors and assignment operators are also deleted
		ServiceLocator(ServiceLocator&) = delete;
		ServiceLocator(ServiceLocator&&) = delete;

		ServiceLocator& operator=(ServiceLocator&) = delete;
		ServiceLocator& operator=(ServiceLocator&&) = delete;

		//------------------
		// File system
		//------------------

		/// <summary>
		/// Register a new IFileSystem interface
		/// </summary>
		/// <param name="pFileSystem">unique pointer to the new IFileSystem interface</param>
		static void RegisterFileSystem(std::unique_ptr<Filesystem::IFileSystem> pFileSystem);

		/// <summary>
		/// Get a reference to the IFileSystem interface
		/// </summary>
		/// <returns>Reference to the IFileSystem interface</returns>
		static Filesystem::IFileSystem& GetFileSystem();


		//------------------
		// Renderer
		//------------------

		/// <summary>
		/// Register a new IRenderer interface
		/// </summary>
		/// <param name="pRenderer">unique pointer to the new IRenderer interface</param>
		static void RegisterRenderer(std::unique_ptr<Rendering::IRenderer> pRenderer);

		/// <summary>
		/// Get a reference to the IRenderer interface
		/// </summary>
		/// <returns>Reference to the IRenderer interface</returns>
		static Rendering::IRenderer& GetRenderer();

	private:
		// Default file system instance
		static std::unique_ptr<Filesystem::DefaultFileSystem> m_pDefaultFileSystemInstance;
		// Current file system instance
		static std::unique_ptr<Filesystem::IFileSystem> m_pFileSystemInstance;

		// Default renderer instance
		static std::unique_ptr<Rendering::DefaultRenderer> m_pDefaultRendererInstance;
		// Current renderer instance
		static std::unique_ptr<Rendering::IRenderer> m_pRendererInstance;
	};
}

#endif // !_DDM_SERVICE_LOCATOR_
