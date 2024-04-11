// VulkanRenderer2D
// This file will act as the main renderer, it holds all the objects needed for rendering

#ifndef VulkanRenderer2DIncluded
#define VulkanRenderer2DIncluded

// File includes
#include "Singleton.h"

// Standard library includes
#include <memory>

namespace D2D
{
	// Class forward declarations
	class InstanceWrapper;

	class VulkanRenderer2D final : public Singleton<VulkanRenderer2D>
	{
	public:
		VulkanRenderer2D();

		~VulkanRenderer2D();

	private:
		std::unique_ptr<InstanceWrapper> m_pInstanceWrapper{};

		void InitVulkan();

		void CleanupVulkan();

	};
}


#endif // !VulkanRenderer2DIncluded