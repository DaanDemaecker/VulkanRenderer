#ifndef GPUObjectIncluded
#define GPUObjectIncluded

// File includes
#include "../../Includes/VulkanIncludes.h"

// Standard library includes
#include <vector>

namespace D2D
{
	class GPUObject
	{
	public:
		GPUObject() = delete;

		GPUObject(const VkInstance& instance);

		~GPUObject() = default;



	private:
		VkPhysicalDevice m_PhysicalDevice{};

		void SetupPhysicalDevice(const VkInstance& instance);

		void EnumeratePhysicalDevices(const VkInstance& instance, std::vector<VkPhysicalDevice>& devices);
	};
}

#endif // !GPUObjectIncluded