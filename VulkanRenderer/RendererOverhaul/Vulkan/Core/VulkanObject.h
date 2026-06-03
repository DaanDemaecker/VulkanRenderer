// VulkanObject.h
// This class serves as a base class for all vulkan objects.
// It holds the common members and functions that all vulkan objects have such as the core object and the allocator

#ifndef _DDM_VULKAN_OBJECT_
#define _DDM_VULKAN_OBJECT_

// Standard library includes
#include <stdexcept>

// File includes
#include "Vulkan/Core/VulkanAllocator.h"
#include "Vulkan/Core/VulkanCore.h"


namespace DDM::Vulkan
{
	class VulkanObject
	{
	public:
		// Default constructor
		VulkanObject() = delete;

		/// <summary>
		/// Virtual destructor
		/// </summary>
		virtual ~VulkanObject() = default;

		// Delete copy and move constructors
		VulkanObject(VulkanObject&) = delete;
		VulkanObject(VulkanObject&&) = delete;

		// Delete copy and move assignment operators
		VulkanObject& operator=(VulkanObject&) = delete;
		VulkanObject& operator=(VulkanObject&&) = delete;

	protected:
		// Protected constructor to avoid creation of this baseclass

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="pCore"></param>
		/// <param name="pAllocator"></param>
		VulkanObject(const VulkanCore* pCore, const VulkanAllocator* pAllocator)
			:m_pAllocator(pAllocator),
			m_pCore(pCore)
		{
			if (m_pAllocator == nullptr)
			{
				throw std::runtime_error("VulkanObject created with null allocator");
			}
			if (m_pCore == nullptr)
			{
				throw std::runtime_error("VulkanObject created with null core");
			}
		}

		// Pointer to the vulkan allocator
		const VulkanAllocator* m_pAllocator;

		// Pointer to the vulkan core object
		const VulkanCore* m_pCore;
	};
}

#endif // !_DDM_VULKAN_OBJECT_
