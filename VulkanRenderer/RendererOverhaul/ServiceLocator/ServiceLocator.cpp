// ServiceLocator.cpp

// Header include
#include "ServiceLocator.h"

// File includes

// Standard library includes

// Static members initialization
std::unique_ptr<DDM::Filesystem::DefaultFileSystem> DDM::ServiceLocator::m_pDefaultFileSystemInstance{ std::make_unique<DDM::Filesystem::DefaultFileSystem>() };
std::unique_ptr<DDM::Filesystem::IFileSystem> DDM::ServiceLocator::m_pFileSystemInstance{ nullptr };

std::unique_ptr<DDM::Rendering::DefaultRenderer> DDM::ServiceLocator::m_pDefaultRendererInstance{ std::make_unique<DDM::Rendering::DefaultRenderer>() };
std::unique_ptr<DDM::Rendering::IRenderer> DDM::ServiceLocator::m_pRendererInstance{ nullptr };

void DDM::ServiceLocator::RegisterFileSystem(std::unique_ptr<DDM::Filesystem::IFileSystem> pFileSystem)
{
	if (pFileSystem != nullptr)
	{
		m_pFileSystemInstance = std::move(pFileSystem);
	}
}

DDM::Filesystem::IFileSystem& DDM::ServiceLocator::GetFileSystem()
{
	return m_pFileSystemInstance == nullptr ? *m_pDefaultFileSystemInstance.get() : *m_pFileSystemInstance.get();
}

void DDM::ServiceLocator::RegisterRenderer(std::unique_ptr<DDM::Rendering::IRenderer> pRenderer)
{
	if (pRenderer != nullptr)
	{
		m_pRendererInstance = std::move(pRenderer);
	}
}

DDM::Rendering::IRenderer& DDM::ServiceLocator::GetRenderer()
{
	return m_pRendererInstance == nullptr ? *m_pDefaultRendererInstance.get() : *m_pRendererInstance.get();
}
