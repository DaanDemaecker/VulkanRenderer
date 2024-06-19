
#ifndef ViewportIncluded
#define ViewPortIncluded

// File includes
#include "VulkanIncludes.h"

// Standard library includes

namespace D3D
{
	class Viewport final
	{
	public:
		Viewport();

		~Viewport() = default;

		void SetViewport(VkCommandBuffer commandBuffer, VkExtent2D swapchainExtent);

	private:
		VkViewport m_Viewport{};

		VkRect2D m_Scissor{};

	};
}
#endif // !ViewportIncluded