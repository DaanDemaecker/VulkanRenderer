// VulkanAllocator.cpp

// Header include
#include "VulkanAllocator.h"

// Standard library includes
#include <iostream>


DDM::VulkanAllocator::VulkanAllocator()
{
	m_VkAllocationCallbacks.pUserData = static_cast<void*>(this);
	m_VkAllocationCallbacks.pfnAllocation = Allocation;
	m_VkAllocationCallbacks.pfnReallocation = Reallocation;
	m_VkAllocationCallbacks.pfnFree = Free;
	m_VkAllocationCallbacks.pfnInternalAllocation = nullptr;
	m_VkAllocationCallbacks.pfnInternalFree = nullptr;
}

DDM::VulkanAllocator::~VulkanAllocator()
{

}

const VkAllocationCallbacks* DDM::VulkanAllocator::GetAllocator() const
{
	return &m_VkAllocationCallbacks;
}

// ------------------------------------------------------------------------------
// Static functions
// ------------------------------------------------------------------------------


void* VKAPI_CALL DDM::VulkanAllocator::Allocation(void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
	return static_cast<DDM::VulkanAllocator*>(pUserData)->Allocation(size, alignment, allocationScope);
}

void* VKAPI_CALL DDM::VulkanAllocator::Reallocation(void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
	return static_cast<DDM::VulkanAllocator*>(pUserData)->Reallocation(pOriginal, size, alignment, allocationScope);
}


void VKAPI_CALL DDM::VulkanAllocator::Free(void* pUserData, void* pMemory)
{
	static_cast<DDM::VulkanAllocator*>(pUserData)->Free(pMemory);
}

// ------------------------------------------------------------------------------
// Nonstatic functions
// ------------------------------------------------------------------------------

void* DDM::VulkanAllocator::Allocation(size_t size, size_t alignment, VkSystemAllocationScope /*allocationScope*/)
{
	return _aligned_malloc(size, alignment);
}

void* DDM::VulkanAllocator::Reallocation(void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope /*allocationScope*/)
{
	return _aligned_realloc(pOriginal, size, alignment);
}

void DDM::VulkanAllocator::Free(void* pMemory)
{
	_aligned_free(pMemory);
}
