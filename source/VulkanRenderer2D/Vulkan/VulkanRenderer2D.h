// VulkanRenderer2D
// This file will act as the main renderer, it holds all the objects needed for rendering

#ifndef VulkanRenderer2DIncluded
#define VulkanRenderer2DIncluded

// File includes
#include "../Engine Files/Singleton.h"

// Standard library includes
#include <memory>

namespace D2D
{
	// Class forward declarations
	class InstanceWrapper;
	class GPUObject;

	class VulkanRenderer2D final : public Singleton<VulkanRenderer2D>
	{
	public:
		VulkanRenderer2D();

		~VulkanRenderer2D();

	private:
		std::unique_ptr<InstanceWrapper> m_pInstanceWrapper{};

		std::unique_ptr<GPUObject> m_pGPUObject{};

		void InitVulkan();

		void CleanupVulkan();

	};
}


#endif // !VulkanRenderer2DIncluded