// Vulkan2D.cpp

// Header include
#include "Vulkan2D.h"

DDM2::Vulkan2D::Vulkan2D()
{
	m_pDispatchableManager = std::make_unique<DDM2::DispatchableManager>();
}
DDM2::Vulkan2D::~Vulkan2D()
{

}

void DDM2::Vulkan2D::Init()
{
}

void DDM2::Vulkan2D::Terminate()
{
}

DDM2::DispatchableManager* DDM2::Vulkan2D::GetDispatchableManager() const
{
	return m_pDispatchableManager.get();
}

