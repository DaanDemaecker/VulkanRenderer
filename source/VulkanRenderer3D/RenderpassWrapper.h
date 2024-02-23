// RenerpassWrapper.h
// This class will serve as a wrapper for the VkRenderPass object

#ifndef RenderpassWrapperIncluded
#define RenderpassWrapperIncluded

// File includdes
#include "GLFWIncludes.h"

namespace D3D
{
	class RenderpassWrapper
	{
	public:
		RenderpassWrapper() = default;
		~RenderpassWrapper() = default;

		RenderpassWrapper(RenderpassWrapper& other) = delete;
		RenderpassWrapper(RenderpassWrapper&& other) = delete;

		RenderpassWrapper& operator=(RenderpassWrapper& other) = delete;
		RenderpassWrapper& operator=(RenderpassWrapper&& other) = delete;
	
	private:
	};
}

#endif // !RenderpassWrapperIncluded
