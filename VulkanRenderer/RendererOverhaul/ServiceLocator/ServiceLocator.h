// ServiceLocator.h

#ifndef _DDM_SERVICE_LOCATOR_
#define _DDM_SERVICE_LOCATOR_

// File includes

// Standard library includes
#include <memory>

namespace DDM
{
	class ServiceLocator final
	{
	public:
		ServiceLocator() = delete;

		~ServiceLocator();

		ServiceLocator(ServiceLocator&) = delete;
		ServiceLocator(ServiceLocator&&) = delete;

		ServiceLocator& operator=(ServiceLocator&) = delete;
		ServiceLocator& operator=(ServiceLocator&&) = delete;

	private:

	};
}

#endif // !_DDM_SERVICE_LOCATOR_
