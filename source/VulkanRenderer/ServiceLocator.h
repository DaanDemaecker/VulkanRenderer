#pragma once
#include <memory>
#include "Renderer.h"

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
		
		static Renderer& GetRenderer();
		static void RegisterRenderer(std::unique_ptr<Renderer> renderer);


	private:
		static std::unique_ptr<Renderer> m_pRenderer;
		static std::unique_ptr<DefaultRenderer> m_pDefaultRenderer;
	};
}