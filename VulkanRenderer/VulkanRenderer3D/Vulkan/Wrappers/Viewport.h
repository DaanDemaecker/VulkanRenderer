
#ifndef ViewportIncluded
#define ViewPortIncluded

// File includes
#include "Includes/VulkanIncludes.h"

// Standard library includes

namespace DDM3
{
	class Viewport final
	{
	public:
		Viewport();

		Viewport(VkExtent2D viewportSize);

		~Viewport() = default;

		void SetViewport(VkCommandBuffer commandBuffer);

		void SetViewportAndScissor(VkExtent2D rect);

		void SetViewportSize(VkExtent2D viewportSize);

		void SetScissor(VkExtent2D extent);

	private:
		VkViewport m_Viewport{};

		VkRect2D m_Scissor{};

	};
}
#endif // !ViewportIncluded