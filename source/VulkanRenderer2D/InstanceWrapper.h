// InstanceWrapper
// This file will hold and manage all objects concerning the vulkan instance

#ifndef InstanceWrapperIncluded
#define InstanceWrapperIncluded

// File includes
#include "VulkanIncludes.h"

namespace D2D
{
	class InstanceWrapper  final
	{
	public:
		InstanceWrapper();

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