// ServiceLocator.cpp

// Header include
#include "ServiceLocator.h"

// File includes

// Standard library includes

// Static members initialization
std::unique_ptr<DDM::DefaultWindow> DDM::ServiceLocator::m_pDefaultWindowInstance{ std::make_unique<DefaultWindow>() };
std::unique_ptr<DDM::IWindow> DDM::ServiceLocator::m_pWindowInstance{ nullptr };

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
