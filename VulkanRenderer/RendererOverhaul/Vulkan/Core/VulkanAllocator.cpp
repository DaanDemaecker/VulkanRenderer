// VulkanAllocator.cpp

// Header include
#include "VulkanAllocator.h"

// Standard library includes
#include <iostream>


DDM::Vulkan::VulkanAllocator::VulkanAllocator()
{
	m_VkAllocationCallbacks.pUserData = static_cast<void*>(this);
	m_VkAllocationCallbacks.pfnAllocation = Allocation;
	m_VkAllocationCallbacks.pfnReallocation = Reallocation;
	m_VkAllocationCallbacks.pfnFree = Free;
	m_VkAllocationCallbacks.pfnInternalAllocation = nullptr;
	m_VkAllocationCallbacks.pfnInternalFree = nullptr;
}

DDM::Vulkan::VulkanAllocator::~VulkanAllocator()
{

}

const VkAllocationCallbacks* DDM::Vulkan::VulkanAllocator::GetAllocator() const
{
	return &m_VkAllocationCallbacks;
}

// ------------------------------------------------------------------------------
// Static functions
// ------------------------------------------------------------------------------


void* VKAPI_CALL DDM::Vulkan::VulkanAllocator::Allocation(void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
	return static_cast<DDM::Vulkan::VulkanAllocator*>(pUserData)->Allocation(size, alignment, allocationScope);
}

void* VKAPI_CALL DDM::Vulkan::VulkanAllocator::Reallocation(void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
	return static_cast<DDM::Vulkan::VulkanAllocator*>(pUserData)->Reallocation(pOriginal, size, alignment, allocationScope);
}


void VKAPI_CALL DDM::Vulkan::VulkanAllocator::Free(void* pUserData, void* pMemory)
{
	static_cast<DDM::Vulkan::VulkanAllocator*>(pUserData)->Free(pMemory);
}

// ------------------------------------------------------------------------------
// Nonstatic functions
// ------------------------------------------------------------------------------

void* DDM::Vulkan::VulkanAllocator::Allocation(size_t size, size_t alignment, VkSystemAllocationScope /*allocationScope*/)
{
	return _aligned_malloc(size, alignment);
}

void* DDM::Vulkan::VulkanAllocator::Reallocation(void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope /*allocationScope*/)
{
	return _aligned_realloc(pOriginal, size, alignment);
}

void DDM::Vulkan::VulkanAllocator::Free(void* pMemory)
{
	_aligned_free(pMemory);
}
