// VulkanRenderer.cpp

// Header include
#include "VulkanRenderer.h"
#include "Vulkan/VulkanImplementation.h"

//------------------------------------------------------------------------
// VulkanImpl
//------------------------------------------------------------------------
namespace DDM::Rendering
{
	class VulkanImpl
	{
	public:
		VulkanImpl()
		{
			m_pVulkanObject = std::make_unique<DDM::Vulkan::VulkanImplementation>();
		}

		~VulkanImpl()
		{

		}

		VulkanImpl(VulkanImpl&) = delete;
		VulkanImpl(VulkanImpl&&) = delete;

		VulkanImpl& operator=(VulkanImpl&) = delete;
		VulkanImpl& operator=(VulkanImpl&&) = delete;

		void Init()
		{

		}

		void Render()
		{

		}

		void Terminate()
		{

		}

	private:
		std::unique_ptr<DDM::Vulkan::VulkanImplementation> m_pVulkanObject{};
	};
}
//------------------------------------------------------------------------


//------------------------------------------------------------------------
// VulkanRenderer
//------------------------------------------------------------------------
DDM::Rendering::VulkanRenderer::VulkanRenderer()
{
	m_pImpl = std::make_unique<VulkanImpl>();
}

DDM::Rendering::VulkanRenderer::~VulkanRenderer()
{
}

void DDM::Rendering::VulkanRenderer::Init()
{
	m_pImpl->Init();
}

void DDM::Rendering::VulkanRenderer::Render()
{
	m_pImpl->Render();
}

void DDM::Rendering::VulkanRenderer::Terminate()
{
	m_pImpl->Terminate();
}
//------------------------------------------------------------------------
