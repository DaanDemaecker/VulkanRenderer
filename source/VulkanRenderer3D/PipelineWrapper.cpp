#include "PipelineWrapper.h"

D3D::PipelineWrapper::PipelineWrapper()
{
}

void D3D::PipelineWrapper::Cleanup(VkDevice device)
{
	// Destroy the pipeline
	vkDestroyPipeline(device, m_Pipeline, nullptr);
	//Destroy the pipeline layout
	vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr);
}
