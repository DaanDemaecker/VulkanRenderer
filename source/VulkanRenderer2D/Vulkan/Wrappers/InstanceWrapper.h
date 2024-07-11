// InstanceWrapper
// This file will hold and manage all objects concerning the vulkan instance

#ifndef InstanceWrapperIncluded
#define InstanceWrapperIncluded

// File includes
#include "../../Includes/VulkanIncludes.h"

namespace D2D
{
	class InstanceWrapper  final
	{
	public:
		InstanceWrapper();

		// Get a reference to the vulkan instance
		const VkInstance& GetInstance() const { return m_Instance; }

	private:
		// Vulkan instance handle
		VkInstance m_Instance{};

		// Create the instance
		void CreateInstance();

		// Set up application info
		// Parameters:
		//     info: reference to a application info struct
		void SetupApplicationInfo(VkApplicationInfo& info);
	};
}

#endif // !InstanceWrapperIncluded