// Vulkan2D.cpp

// Header include
#include "Vulkan2D.h"

D2D::Vulkan2D::Vulkan2D()
{
	m_pDispatchableManager = std::make_unique<D2D::DispatchableManager>();
}
D2D::Vulkan2D::~Vulkan2D()
{

}

void D2D::Vulkan2D::Init()
{
}

void D2D::Vulkan2D::Terminate()
{
}

D2D::DispatchableManager* D2D::Vulkan2D::GetDispatchableManager() const
{
	return m_pDispatchableManager.get();
}

