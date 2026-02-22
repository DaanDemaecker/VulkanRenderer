// VulkanRenderer.h
// This file will hold the VulkanRenderer class declaration, which will be the implementation of the IRenderer interface for the Vulkan API

#ifndef _DDM_VULKAN_RENDERER_
#define _DDM_VULKAN_RENDERER_

// Parent include
#include "ServiceLocator/Renderer/IRenderer.h"

// Standard library includes
#include <memory>

namespace DDM
{
	class VulkanImpl;

	class VulkanRenderer final : public IRenderer
	{
	public:
		VulkanRenderer();
		~VulkanRenderer() override;

		void Init() override;
		void Render() override;
		void Terminate() override;

	private:
		std::unique_ptr<VulkanImpl> m_pImpl;
	};
}

#endif // !_DDM_VULKAN_RENDERER_
