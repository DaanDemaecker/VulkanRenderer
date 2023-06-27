#pragma once

namespace D3D
{
	class ServiceLocator
	{
	public:
		ServiceLocator() = default;
		~ServiceLocator() = default;

		ServiceLocator(ServiceLocator& other) = delete;
		ServiceLocator(ServiceLocator&& other) = delete;
		ServiceLocator operator=(ServiceLocator& other) = delete;
		ServiceLocator operator=(ServiceLocator&& other) = delete;
		
	private:

	};
}