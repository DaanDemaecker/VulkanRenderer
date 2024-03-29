// PipelineManager.cpp

// File includes
#include "PipelineManager.h"
#include "Utils.h"
#include "ConfigManager.h"
#include "ShaderModuleWrapper.h"
#include "PipelineWrapper.h"

D3D::PipelineManager::PipelineManager()
{
}

void D3D::PipelineManager::Cleanup(VkDevice device)
{
	// Loop trough all the pipelines
	for (auto& pipeline : m_GraphicPipelines)
	{
		// Call cleanup function
		pipeline.second->Cleanup(device);
	}
}

void D3D::PipelineManager::AddDefaultPipeline(VkDevice device, VkRenderPass renderPass, VkSampleCountFlagBits sampleCount)
{
	// Get config manager
	auto& configManager{ ConfigManager::GetInstance() };

	// Initialize default pipeline name 
	m_DefaultPipelineName = configManager.GetString("DefaultPipelineName");

	// Add default pipeline
	AddGraphicsPipeline(device, renderPass, sampleCount, m_DefaultPipelineName, {
		configManager.GetString("DefaultVertName"),
		configManager.GetString("DefaultFragName") });

}

void D3D::PipelineManager::AddGraphicsPipeline(VkDevice device, VkRenderPass renderPass, VkSampleCountFlagBits sampleCount, const std::string& pipelineName, std::initializer_list<const std::string>&& filePaths, bool hasDepthStencil)
{
	AddGraphicsPipeline(device, renderPass, sampleCount, pipelineName, filePaths, hasDepthStencil);
}

void D3D::PipelineManager::AddGraphicsPipeline(VkDevice device, VkRenderPass renderPass, VkSampleCountFlagBits sampleCount, const std::string& pipelineName, std::initializer_list<const std::string>& filePaths, bool hasDepthStencil)
{
	// Check if pipeline already exists, if it does, delete it
	if (m_GraphicPipelines.contains(pipelineName))
	{
		m_GraphicPipelines[pipelineName]->Cleanup(device);
	}

	m_GraphicPipelines[pipelineName] = std::make_unique<D3D::PipelineWrapper>
		(device, renderPass, sampleCount, filePaths, hasDepthStencil);
	
}

D3D::PipelineWrapper* D3D::PipelineManager::GetPipeline(const std::string& name)
{
	// Check if pipeline exists
	if (m_GraphicPipelines.contains(name))
	{
		// If it exists, return the correct pipeline
		return m_GraphicPipelines[name].get();
	}
	else
	{
		// If not, return default pipeline
		return m_GraphicPipelines[m_DefaultPipelineName].get();
	}
}