// DispatchableManager.cpp

// File includes
#include "DispatchableManager.h"
#include "Wrappers/InstanceWrapper.h"
#include "Wrappers/GPUObject.h"

DDM2::DispatchableManager::DispatchableManager()
{
	m_pInstanceWrapper = std::make_unique<InstanceWrapper>();

	m_pGPUObject = std::make_unique<GPUObject>(m_pInstanceWrapper->GetInstance());
}

DDM2::DispatchableManager::~DispatchableManager()
{

}