// SurfaceWrapper.h
// This class will hold and handle all VkSurfaceKHR stuff

#ifndef SurfaceWrapperIncluded
#define SurfaceWrapperIncluded

// File includes
#include "Includes/VulkanIncludes.h"

namespace DDM3
{
	class SurfaceWrapper final
	{
	public:
		// Constructor
		// Parameters:
		//     instance: The handle of the Vulkan instance
		SurfaceWrapper(VkInstance instance);

		// Delete default constructor
		SurfaceWrapper() = delete;

		// Default destructor
		~SurfaceWrapper() = default;

		// Delete copy and move functions
		SurfaceWrapper(SurfaceWrapper& other) = delete;
		SurfaceWrapper(SurfaceWrapper&& other) = delete;
		SurfaceWrapper& operator=(SurfaceWrapper& other) = delete;
		SurfaceWrapper& operator=(SurfaceWrapper&& other) = delete;

		// Get the handle of the surface
		VkSurfaceKHR GetSurface() const { return m_Surface; }

		// Clean up allocated objects
		// Parameters:
		//     instance: The handle of the Vulkan instance
		void Cleanup(VkInstance instance);
	private:
		// Handle of the VkSurfaceKHR
		VkSurfaceKHR m_Surface{};

		// Initialize the VkSurfaceKHR
		// Parameters:
		//     instance: The handle of the Vulkan instance
		void CreateSurface(VkInstance instance);
	};
}

#endif // !SurfaceWrapperIncluded