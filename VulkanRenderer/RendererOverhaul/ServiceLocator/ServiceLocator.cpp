// ServiceLocator.cpp

// Header include
#include "ServiceLocator.h"

// File includes

// Standard library includes

// Static members initialization
std::unique_ptr<DDM::DefaultWindow> DDM::ServiceLocator::m_pDefaultWindowInstance{ std::make_unique<DefaultWindow>() };
std::unique_ptr<DDM::IWindow> DDM::ServiceLocator::m_pWindowInstance{ nullptr };

std::unique_ptr<DDM::DefaultFileSystem> DDM::ServiceLocator::m_pDefaultFileSystemInstance{ std::make_unique<DDM::DefaultFileSystem>() };
std::unique_ptr<DDM::IFileSystem> DDM::ServiceLocator::m_pFileSystemInstance{ nullptr };

std::unique_ptr<DDM::DefaultRenderer> DDM::ServiceLocator::m_pDefaultRendererInstance{ std::make_unique<DDM::DefaultRenderer>() };
std::unique_ptr<DDM::IRenderer> DDM::ServiceLocator::m_pRendererInstance{ nullptr };

void DDM::ServiceLocator::RegisterWindow(std::unique_ptr<IWindow> pWindow)
{
	if (pWindow != nullptr)
	{
		m_pWindowInstance = std::move(pWindow);
	}
}

DDM::IWindow& DDM::ServiceLocator::GetWindow()
{
	return m_pWindowInstance == nullptr ? *m_pDefaultWindowInstance.get() : *m_pWindowInstance.get();
}

void DDM::ServiceLocator::RegisterFileSystem(std::unique_ptr<IFileSystem> pFileSystem)
{
	if (pFileSystem != nullptr)
	{
		m_pFileSystemInstance = std::move(pFileSystem);
	}
}

DDM::IFileSystem& DDM::ServiceLocator::GetFileSystem()
{
	return m_pFileSystemInstance == nullptr ? *m_pDefaultFileSystemInstance.get() : *m_pFileSystemInstance.get();
}

void DDM::ServiceLocator::RegisterRenderer(std::unique_ptr<IRenderer> pRenderer)
{
	if (pRenderer != nullptr)
	{
		m_pRendererInstance = std::move(pRenderer);
	}
}

DDM::IRenderer& DDM::ServiceLocator::GetRenderer()
{
	return m_pRendererInstance == nullptr ? *m_pDefaultRendererInstance.get() : *m_pRendererInstance.get();
}
