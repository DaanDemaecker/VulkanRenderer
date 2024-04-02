// PipelineManager.h
// This class will handle graphics pipelines

#ifndef PipelineManagerIncluded
#define PipelineManagerIncluded

// File includes
#include "VulkanIncludes.h"
#include "Structs.h"

// Standard library includes
#include <string>
#include <map>
#include <vector>
#include <initializer_list>
#include <memory>

namespace D3D
{
	// Class forward declarations
	class PipelineWrapper;

	class PipelineManager final
	{
	public:
		// Constructor
		PipelineManager();

		// Destructor
		~PipelineManager();

		// Delete copy and move functions
		PipelineManager(PipelineManager& other) = delete;
		PipelineManager(PipelineManager&& other) = delete;
		PipelineManager& operator=(PipelineManager& other) = delete;
		PipelineManager& operator=(PipelineManager&& other) = delete;

		// Add a graphics pipeline to the vector
		// Parameters:
		//     device: the VkDevice handle
		//     renderPass: the handle of the VkRenderpass that will be used
		//     sampleCount: the max useable sample count
		//     pipelineName: the name for this pipeLine
		//     filePaths: a list of shader file names for this pipeline
		//     isSkybox: boolean that indicates if this pipeline needs a depth stencil
		void AddGraphicsPipeline(VkDevice device, VkRenderPass renderPass, VkSampleCountFlagBits sampleCount,
			const std::string& pipelineName, std::initializer_list<const std::string>& filePaths, bool hasDepthStencil = true);

		// Add a graphics pipeline to the vector
		// Parameters:
		//     device: the VkDevice handle
		//     renderPass: the handle of the VkRenderpass that will be used
		//     sampleCount: the max useable sample count
		//     pipelineName: the name for this pipeLine
		//     filePaths: a list of shader file names for this pipeline
		//     isSkybox: boolean that indicates if this pipeline needs a depth stencil
		void AddGraphicsPipeline(VkDevice device, VkRenderPass renderPass, VkSampleCountFlagBits sampleCount,
			const std::string& pipelineName, std::initializer_list<const std::string>&& filePaths, bool hasDepthStencil = true);

		// Add default pipeline to the vector
		// Parameters:
		//     device: the VkDevice handle
		//     renderPass: the handle of the VkRenderpass that will be used
		//     sampleCount: the max useable sample count
		void AddDefaultPipeline(VkDevice device, VkRenderPass renderPass, VkSampleCountFlagBits sampleCount);

		// Get a certain graphics pipeline
		// Parameters:
		//     name: the name of the requested pipeline
		PipelineWrapper* GetPipeline(const std::string& name);

		// Clean up everything
		// Parameters:
		//     device: handle of the VkDevice
		void Cleanup(VkDevice device);

	private:
		// A map of all the graphics pipelines
		// A string is used to as key for the pipelines
		std::map<std::string, std::unique_ptr<PipelineWrapper>> m_GraphicPipelines{};

		// The name of the default pipeline
		std::string m_DefaultPipelineName{};
	};
}

#endif // !PipelineManagerIncluded