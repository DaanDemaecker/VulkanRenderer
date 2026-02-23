// VulkanRenderer.cpp

// Header include
#include "VulkanRenderer.h"
#include "Includes/VulkanIncludes.h"

// VulkanImpl
//------------------
namespace DDM
{
	class VulkanImpl
	{
	public:
		VulkanImpl()
		{

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

	};
}
//------------------


// VulkanRenderer
//------------------
DDM::VulkanRenderer::VulkanRenderer()
{
	m_pImpl = std::make_unique<VulkanImpl>();
}

DDM::VulkanRenderer::~VulkanRenderer()
{
}

void DDM::VulkanRenderer::Init()
{
	m_pImpl->Init();
}

void DDM::VulkanRenderer::Render()
{
	m_pImpl->Render();
}

void DDM::VulkanRenderer::Terminate()
{
	m_pImpl->Terminate();
}
//------------------
