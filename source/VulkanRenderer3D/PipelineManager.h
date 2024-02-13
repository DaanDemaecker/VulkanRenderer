// PipelineManager.h
// This class will handle graphics pipelines

#ifndef PipelineManagerIncluded
#define PipelineManagerIncluded

// File includes
#include "GLFWIncludes.h"
#include "Structs.h"

// Standard library includes
#include <string>
#include <map>
#include <vector>

namespace D3D
{
	class PipelineManager
	{
	public:
		PipelineManager() = delete;

		PipelineManager(const std::string& defaultPipelineName, const std::string& defaultVertName, const std::string& defaultFragName);

		~PipelineManager() = default;

		PipelineManager(PipelineManager& other) = delete;
		PipelineManager(PipelineManager&& other) = delete;

		PipelineManager& operator=(PipelineManager& other) = delete;
		PipelineManager& operator=(PipelineManager&& other) = delete;

		void AddGraphicsPipeline(VkDevice device, uint32_t maxFrames, VkRenderPass renderPass, VkSampleCountFlagBits sampleCount, const std::string& pipelineName, const std::string& vertShaderName, const std::string& fragShaderName, int vertexUbos, int fragmentUbos, int textureAmount);

		std::vector<VkDescriptorSetLayout>& GetDescriptorSetLayout(VkDevice device, uint32_t maxFrames, int vertexUbos, int fragmentUbos, int textureAmount);

		PipelinePair& GetPipeline(const std::string& name);

		void Cleanup(VkDevice device);

	private:
		std::map<std::tuple<int, int, int>, std::vector<VkDescriptorSetLayout>> m_DescriptorSetLayouts{};

		std::map<std::string, PipelinePair> m_GraphicPipelines{};

		const std::string m_DefaultPipelineName;
		const std::string m_DefaultVertName;
		const std::string m_DefaultFragName;

		VkShaderModule CreateShaderModule(VkDevice device, const std::vector<char>& code);

		
		void CreateDescriptorLayout(VkDevice device, uint32_t maxFrames, int vertexUbos, int fragmentUbos, int textureAmount);
	};
}

#endif // !PipelineManagerIncluded