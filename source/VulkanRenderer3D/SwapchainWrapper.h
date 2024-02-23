// SwapchainWrapper.h
// This class will serve as a wrapper for the vulkan swapchain

#ifndef SwapchainWrapperIncluded
#define SwapchainWrapperIncluded

// File includes
#include "GLFWIncludes.h"

namespace D3D
{

	class SwapchainWrapper
	{
	public:
		SwapchainWrapper() = default;
		~SwapchainWrapper() = default;

		SwapchainWrapper(SwapchainWrapper& other) = delete;
		SwapchainWrapper(SwapchainWrapper&& other) = delete;

		SwapchainWrapper& operator=(SwapchainWrapper& other) = delete;
		SwapchainWrapper& operator=(SwapchainWrapper&& other) = delete;

	private:

	};

}
#endif // !SwapchainWrapperIncluded