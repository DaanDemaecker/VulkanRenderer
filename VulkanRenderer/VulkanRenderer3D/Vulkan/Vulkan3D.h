// Vulkan3D.h

#ifndef Vulkan3DIncluded
#define Vulkan3DIncluded

// File includes
#include "Engine/Singleton.h"
#include "Includes/VulkanIncludes.h"

// Standard library includes
#include <memory>

namespace D3D
{
	// Class forward declarations
	class DispatchableManager;

	class GPUObject;

	class Vulkan3D final : public Singleton<Vulkan3D>
	{
	public:
		// Destructor
		~Vulkan3D();
		
		// Delete copy and move functions
		Vulkan3D(Vulkan3D& other) = delete;
		Vulkan3D(Vulkan3D&& other) = delete;
		Vulkan3D& operator=(Vulkan3D& other) = delete;
		Vulkan3D& operator=(Vulkan3D && other) = delete;

		// Initialize the renderer, must be called at start of program
		void Init();

		// Terminate the renderer, must be called at end of program
		void Terminate();

		// Get vulkan instance
		VkInstance GetVulkanInstance() const;

		// Get logical device
		VkDevice GetDevice() const;

		// Get physical device
		VkPhysicalDevice GetPhysicalDevice() const;

		// Get GPU object
		GPUObject* GetGPUObject() const;

		// Get surface
		VkSurfaceKHR GetSurface() const;
	
	private:
		// Private  default constructor accessable for singleton class
		friend class Singleton<Vulkan3D>;
		Vulkan3D();

		// Dispatchable manager
		std::unique_ptr<D3D::DispatchableManager> m_pDispatchableManager{};
	};

}
#endif // !Vulkan3DIncluded