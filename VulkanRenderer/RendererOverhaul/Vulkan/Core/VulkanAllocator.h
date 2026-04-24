// VulkanAllocator.h
// This class will serve as the pAllocator parameter in vullkan create functions

#ifndef _DDM_VULKAN_ALLOCATOR_
#define _DDM_VULKAN_ALLOCATOR_

// File includes
#include "Includes/VulkanIncludes.h"

namespace DDM
{
	class VulkanAllocator final
	{
	public:
		/// <summary>
		/// Default constructor
		/// </summary>
		VulkanAllocator();

		/// <summary>
		/// Destructor
		/// </summary>
		~VulkanAllocator();

		/// <summary>
		/// Get a pointer to the allocationcallback struct
		/// </summary>
		/// <returns>Pointer to the allocationcallback struct</returns>
		const VkAllocationCallbacks* GetAllocator() const;

	private:
		// Allocation callback struct
		VkAllocationCallbacks m_VkAllocationCallbacks{};


		// ------------------------------------------------------------------------------
		// Static functions
		// ------------------------------------------------------------------------------

		/// <summary>
		/// Allocate memory for a vulkan object
		/// </summary>
		/// <param name="pUserData">Data supplied by user</param>
		/// <param name="size">Size of the requested memory block</param>
		/// <param name="alignment">Alignment of the requested memory block</param>
		/// <param name="allocationScope">Scope of the type of memory</param>
		/// <returns>Pointer to the allocated block</returns>
		static void* VKAPI_CALL Allocation(
			void* pUserData,
			size_t size,
			size_t alignment,
			VkSystemAllocationScope allocationScope);
	
		/// <summary>
		/// Reallocation of a vulkan object
		/// </summary>
		/// <param name="pUserData">Data supplied by user</param>
		/// <param name="pOriginal">Pointer to the original block of memory</param>
		/// <param name="size">Size of the requested memory block</param>
		/// <param name="alignment">Alignment of the requested memory block</param>
		/// <param name="allocationScope">Scope of the type of memory</param>
		/// <returns>Pointer to the newly allocated block</returns>
		static void* VKAPI_CALL Reallocation(
			void* pUserData, 
			void* pOriginal,
			size_t size,
			size_t alignment,
			VkSystemAllocationScope allocationScope);

		/// <summary>
		/// Free a vulkan object
		/// </summary>
		/// <param name="pUserData">Data supplied by user</param>
		/// <param name="pMemory">Pointer to the block of memory to be freed</param>
		/// <returns></returns>
		static void VKAPI_CALL Free(
			void* pUserData,
			void* pMemory);

		// ------------------------------------------------------------------------------
		// Nonstatic functions
		// ------------------------------------------------------------------------------


		/// <summary>
		/// Allocate memory for a vulkan object
		/// </summary>
		/// <param name="size">Size of the requested memory block</param>
		/// <param name="alignment">Alignment of the requested memory block</param>
		/// <param name="allocationScope">Scope of the type of memory</param>
		/// <returns>Pointer to the allocated block</returns>
		void* Allocation(
			size_t size,
			size_t alignment,
			VkSystemAllocationScope allocationScope);

		/// <summary>
		/// Reallocation of a vulkan object
		/// </summary>
		/// <param name="pOriginal">Pointer to the original block of memory</param>
		/// <param name="size">Size of the requested memory block</param>
		/// <param name="alignment">Alignment of the requested memory block</param>
		/// <param name="allocationScope">Scope of the type of memory</param>
		/// <returns>Pointer to the newly allocated block</returns>
		void* Reallocation(
			void* pOriginal,
			size_t size,
			size_t alignment,
			VkSystemAllocationScope allocationScope);

		/// <summary>
		/// Free a vulkan object
		/// </summary>
		/// <param name="pMemory">Pointer to the block of memory to be freed</param>
		/// <returns></returns>
		void Free(
			void* pMemory);
	};
}

#endif // !_DDM_VULKAN_ALLOCATOR_
