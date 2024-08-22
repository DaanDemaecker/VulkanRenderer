// DispatchableManager
// This file hold, manage and handle all dispatchable Vulkan objects

#ifndef VulkanRenderer2DIncluded
#define VulkanRenderer2DIncluded

// File includes
#include "Engine/Singleton.h"

// Standard library includes
#include <memory>

namespace D2D
{
	// Class forward declarations
	class InstanceWrapper;
	class GPUObject;

	class DispatchableManager
	{
	public:
		DispatchableManager();

		~DispatchableManager();

	private:
		std::unique_ptr<InstanceWrapper> m_pInstanceWrapper{};

		std::unique_ptr<GPUObject> m_pGPUObject{};

	};
}


#endif // !VulkanRenderer2DIncluded