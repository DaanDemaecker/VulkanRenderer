// PipelineManager.cpp

// File includes
#include "PipelineManager.h"
#include "Engine/ConfigManager.h"
#include "Vulkan/Vulkan3D.h"
#include "Vulkan/Wrappers/PipelineWrapper.h"
#include "Vulkan/Wrappers/DescriptorPoolWrapper.h"

DDM3::PipelineManager::PipelineManager()
{
}

DDM3::PipelineManager::~PipelineManager()
{
	Cleanup(Vulkan3D::GetInstance().GetDevice());
}

void DDM3::PipelineManager::Cleanup(VkDevice device)
{
	
}

void DDM3::PipelineManager::AddDefaultPipeline(VkDevice device, VkRenderPass renderPass, VkSampleCountFlagBits sampleCount)
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

void DDM3::PipelineManager::AddGraphicsPipeline(VkDevice device, VkRenderPass renderPass, VkSampleCountFlagBits sampleCount, const std::string& pipelineName, std::initializer_list<const std::string>&& filePaths, bool hasDepthStencil)
{
	AddGraphicsPipeline(device, renderPass, sampleCount, pipelineName, filePaths, hasDepthStencil);
}

void DDM3::PipelineManager::AddGraphicsPipeline(VkDevice device, VkRenderPass renderPass, VkSampleCountFlagBits sampleCount, const std::string& pipelineName, std::initializer_list<const std::string>& filePaths, bool hasDepthStencil)
{
	// Check if pipeline already exists, if it does, delete it
	if (m_GraphicPipelines.contains(pipelineName))
	{
		m_GraphicPipelines[pipelineName] = nullptr;
	}

	// Create a new pipeline in the correct spot in the map
	m_GraphicPipelines[pipelineName] = std::make_unique<DDM3::PipelineWrapper>
		(device, renderPass, sampleCount, filePaths, hasDepthStencil);
	
}

DDM3::PipelineWrapper* DDM3::PipelineManager::GetPipeline(const std::string& name)
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