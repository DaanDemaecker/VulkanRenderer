// DispatchableManagerIncluded
// This object will hold and manage dispatchable Vulkan object, this contains the Vulkan instance, physical device, device, commandbuffer and queue

#ifndef DispatchableManagerIncluded
#define DispatchableManagerIncluded

// File includes
#include "Includes/VulkanIncludes.h"

// Standard library includes
#include <memory>

namespace D3D
{
	// Class forward declarations
	class InstanceWrapper;

	class DispatchableManager
	{
	public:
		// Default constructor
		DispatchableManager();

		// Default destructor
		~DispatchableManager();

		// Delete copy and move functions
		DispatchableManager(DispatchableManager& other) = delete;
		DispatchableManager(DispatchableManager&& other) = delete;

		DispatchableManager& operator=(DispatchableManager& other) = delete;
		DispatchableManager& operator=(DispatchableManager&& other) = delete;

		VkInstance GetInstance() const;
		InstanceWrapper* GetInstanceWrapper() const;

	private:
		// Instance wrapper
		std::unique_ptr<InstanceWrapper> m_pInstanceWrapper{};

	};
}

#endif // !DispatchableManagerIncluded
