// ServiceLocator.h

#ifndef _DDM_SERVICE_LOCATOR_
#define _DDM_SERVICE_LOCATOR_

// File includes
#include "Window/IWindow.h"

// Standard library includes
#include <memory>

namespace DDM
{
	class ServiceLocator final
	{
	public:
		ServiceLocator() = delete;

		~ServiceLocator() = default;

		ServiceLocator(ServiceLocator&) = delete;
		ServiceLocator(ServiceLocator&&) = delete;

		ServiceLocator& operator=(ServiceLocator&) = delete;
		ServiceLocator& operator=(ServiceLocator&&) = delete;

		static void RegisterWindow(std::unique_ptr<IWindow> pWindow);
		static IWindow& GetWindow();

	private:
		static std::unique_ptr<DefaultWindow> m_pDefaultWindowInstance;
		static std::unique_ptr<IWindow> m_pWindowInstance;
	};
}

#endif // !_DDM_SERVICE_LOCATOR_
