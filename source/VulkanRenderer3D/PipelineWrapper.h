// PipelineWrapper.cpp
// This class will wrap all the objects necesarry for a pipeline in one class and manage them

#ifndef PipelineWrapperIncluded
#define PipelineWrapperIncluded

// File includes
#include "VulkanIncludes.h"

namespace D3D
{
	class PipelineWrapper
	{
	public:
		PipelineWrapper();

		~PipelineWrapper() = default;

		PipelineWrapper(PipelineWrapper& other) = delete;
		PipelineWrapper(PipelineWrapper&& other) = delete;

		PipelineWrapper& operator=(PipelineWrapper& other) = delete;
		PipelineWrapper& operator=(PipelineWrapper&& other) = delete;

		void Cleanup(VkDevice device);

		void SetPipeline(VkPipeline pipeline) { m_Pipeline = pipeline; }
		void SetPipelineLayout(VkPipelineLayout layout) { m_PipelineLayout = layout; }

		VkPipeline GetPipeline() const { return m_Pipeline; }
		VkPipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }

	private:
		// Pipeline
		VkPipeline m_Pipeline{};
		// Pipeline layout
		VkPipelineLayout m_PipelineLayout{};
	};
}

#endif // !PipelineWrapperIncluded