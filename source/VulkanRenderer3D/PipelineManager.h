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

namespace D3D
{
	class PipelineManager final
	{
	public:
		// Constructor
		PipelineManager();

		// Default destructor
		~PipelineManager() = default;

		PipelineManager(PipelineManager& other) = delete;
		PipelineManager(PipelineManager&& other) = delete;

		PipelineManager& operator=(PipelineManager& other) = delete;
		PipelineManager& operator=(PipelineManager&& other) = delete;

		// Add a graphics pipeline to the vector
		// Parameters:
		//     device: the VkDevice handle
		//     maxFrames: the max amount of frames in flight
		//     renderPass: the handle of the VkRenderpass that will be used
		//     sampleCount: the max useable sample count
		//     pipelineName: the name for this pipeLine
		//     vertShaderName: the filepath to the vertex shader
		//     fragShaderName: the filepath to the fragment shader
		//     vertexUbos: the amount of uniform buffer objects used in the vertex shader
		//     fragmentUbos: the amount of uniform buffer objects used in thes fragment shader
		//     textureAmount: the amount of textures used in the fragment shader
		//     isSkybox: boolean that indicates if this pipeline is for the skybox or not
		void AddGraphicsPipeline(VkDevice device, uint32_t maxFrames, VkRenderPass renderPass, VkSampleCountFlagBits sampleCount,
			const std::string& pipelineName, const std::string& vertShaderName, const std::string& fragShaderName,
			int vertexUbos, int fragmentUbos, int textureAmount, bool isSkybox = false);

		// Add default pipeline to the vector
		// Parameters:
		//     device: the VkDevice handle
		//     maxFrames: the max amount of frames in flight
		//     renderPass: the handle of the VkRenderpass that will be used
		//     sampleCount: the max useable sample count
		void AddDefaultPipeline(VkDevice device, uint32_t maxFrames, VkRenderPass renderPass, VkSampleCountFlagBits sampleCount);

		// Get a certain graphics pipeline
		// Parameters:
		//     name: the name of the requested pipeline
		PipelinePair& GetPipeline(const std::string& name);

		// Get the descriptor set layouts
		// Parameters:
		//     device: handle of the VkDevice
		//     maxFrames: the max amount of frames in flight
		//     vertexUbos: the amount of uniform buffer objects in the vertex shader
		//     fragmentUbos: the amount of uniform buffer objects in the fragment shader
		//     textureAmount: the amount of textures used in the fragment shader
		std::vector<VkDescriptorSetLayout>& GetDescriptorSetLayout(VkDevice device, uint32_t maxFrames, int vertexUbos, int fragmentUbos, int textureAmount);

		// Clean up everything
		// Parameters:
		//     device: handle of the VkDevice
		void Cleanup(VkDevice device);

	private:
		// A map of descriptorset layouts
		// A tuple is used to indicate the amount of vertexubos, fragmentubos and textures respectively
		std::map<std::tuple<int, int, int>, std::vector<VkDescriptorSetLayout>> m_DescriptorSetLayouts{};

		// A map of all the graphics pipelines
		// A string is used to as key for the pipelines
		std::map<std::string, PipelinePair> m_GraphicPipelines{};

		// The name of the default pipeline
		std::string m_DefaultPipelineName{};

		// The name of the vertex funtion
		std::string m_VertexFunction;

		// The name of the vertex funtion
		std::string m_FragmenFunction;

		// Create a new descriptor layout
		// Parameters:
		//     device: handle of the VkDevice
		//     maxFrames: the max amount of frames in flight
		//     vertexUbos: the amount of uniform buffer objects in the vertex shader
		//     fragmentUbos: the amount of uniform buffer objects in the fragment shader
		//     textureAmount: the amount of textures used in the fragment shader
		void CreateDescriptorLayout(VkDevice device, uint32_t maxFrames, int vertexUbos, int fragmentUbos, int textureAmount);

		// Create a vulkan shader module from binary code
		// Parameters:
		//     device: handle of the VkDevice
		//     code: the binary code of the shader
		VkShaderModule CreateShaderModule(VkDevice device, const std::vector<char>& code);

		// Set up vertex input state create info
		// Parameters:
		//     vertexInputStateInfo: a reference to the vertex input state create info to avoid creating a new one in the function
		void SetupVertexInputState(VkPipelineVertexInputStateCreateInfo& vertexInputInfo, VkVertexInputBindingDescription* bindingDescription,
				std::vector<VkVertexInputAttributeDescription>& attributeDescriptions);

		// Set up the rasterizer
		// Parameters:
		//     rasterizer: a reference to the rasterization state create info to avoid creating a new one in the the function
		void SetupRasterizer(VkPipelineRasterizationStateCreateInfo& rasterizer);

		// Set the values of the sample state create info
		// Parameters:
		//     multiSampling: a reference to the multisample state create info to avoid creating a new one in the function
		//     sampleCount: the amount of rasterization samples
		void SetMultisampleStateCreateInfo(VkPipelineMultisampleStateCreateInfo& multisampling, VkSampleCountFlagBits& sampleCount);

		// Set the values of the depth stencil state create info
		// Parameters:
		//     depthStencil: a reference to the septh stencil state create info to avoid creating a new one in the function
		//     isSkybox: boolean that indicates is this depth stencil create info is for the skybox or not
		void SetDepthStencilStateCreateInfo(VkPipelineDepthStencilStateCreateInfo& depthStencil, bool isSkybox);

		// Set the values of the color blend attachment state
		// Parameters:
		//     colorBlendAttachment: a reference to the color blend attachment state to avoid creating a new one in the function
		void SetColorBlendAttachmentState(VkPipelineColorBlendAttachmentState& colorBlendAttachment);

		// Set color blend state create info
		// Parameters:
		//     colorblendStateCreateInfo: a reference to the color blend state info to avoid creating a new one in the function
		//     colorBlendAttachment: a pointer to the color blend attachment needed in the color blend state create info
		void SetColorblendStateCreateInfo(VkPipelineColorBlendStateCreateInfo& colorblendstateCreateInfo,
											VkPipelineColorBlendAttachmentState* colorBlendAttachment);

		// Create pipeline layout create info
		// Parameters:
		//     pipelineLayoutInfo: a reference to the layout create info to avoid creating a new one in the function
		//     device: handle of the VkDevice
		//     maxFrames: the max amount of frames in flight
		//     vertexUbos: the amount of uniform buffer objects in the vertex shader
		//     fragmentUbos: the amount of uniform buffer objects in the fragment shader
		//     textureAmount: the amount of textures used in the fragment shader
		VkPipelineLayoutCreateInfo SetPipelineLayoutCreateInfo(VkPipelineLayoutCreateInfo& pipelineLayoutInfo,VkDevice device, uint32_t maxFrames,
																int vertexUbos, int fragmentUbos, int textureAmount);
	};
}

#endif // !PipelineManagerIncluded