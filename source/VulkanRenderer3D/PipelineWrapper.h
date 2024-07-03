// PipelineWrapper.cpp
// This class will wrap all the objects necesarry for a pipeline in one class and manage them

#ifndef PipelineWrapperIncluded
#define PipelineWrapperIncluded

// File includes
#include "VulkanIncludes.h"
#include "Structs.h"

// Standard library includes
#include <vector>
#include <memory>
#include <string>
#include <initializer_list>

namespace D3D
{
	// Class forward declarations
	class ShaderModuleWrapper;
	class DescriptorPoolWrapper;

	class PipelineWrapper
	{
	public:
		// Delete default constructor
		PipelineWrapper() = delete;

		// Constructor
		// Parameters:
		//     device: handle of the logical device
		//     renerPass: handle of the renderpass
		//     sampleCount: the amount of samples per pixel
		//     filePaths: the filepaths to the shader objects
		//     hasDepthStencil: boolean that indicates if this pipeline needs a depth stencil
		PipelineWrapper(VkDevice device, VkRenderPass renderPass, VkSampleCountFlagBits sampleCount,
			std::initializer_list<const std::string>& filePaths, bool hasDepthStencil = true);

		// Default destructor
		~PipelineWrapper();

		// Delete copy and move functions
		PipelineWrapper(PipelineWrapper& other) = delete;
		PipelineWrapper(PipelineWrapper&& other) = delete;
		PipelineWrapper& operator=(PipelineWrapper& other) = delete;
		PipelineWrapper& operator=(PipelineWrapper&& other) = delete;

		// Clean up all allocated objects
		// Parameters:
		//     device: handle of the logical device
		void Cleanup(VkDevice device);

		// Bind the pipeline
		// Parameters:
		//     commandBuffer: the commandbuffer to be used
		void BindPipeline(VkCommandBuffer commandBuffer);

		// Get a the handle of the pipeline
		VkPipeline GetPipeline() const { return m_Pipeline; }

		// Get the handle of the pipeline layout
		VkPipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }

		// Get the handle of the descriptor layout
		VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_DescriptorSetLayout; }

		// Get a pointer to the descriptor pool wrapper
		D3D::DescriptorPoolWrapper* GetDescriptorPool();

	private:
		// Pipeline
		VkPipeline m_Pipeline{};
		// Pipeline layout
		VkPipelineLayout m_PipelineLayout{};
		// Descriptor set layout
		VkDescriptorSetLayout m_DescriptorSetLayout{};

		// Pointer to the descriptor pool wrapper
		std::unique_ptr<DescriptorPoolWrapper> m_pDescriptorPool{};

		// Create the graphics pipeline
		// Parameters:
		//     device: handle of the VkDevice
		//     renderPass: handle of the VkRenderpass that will be used
		//     sampleCount: max amount of samples per pixel
		//     filePaths: filepaths to all the shader files
		//     hasDepthStencil: boolean that indicates if this pipeline needs a depth stencil
		void CreatePipeline(VkDevice device, VkRenderPass renderPass,
			VkSampleCountFlagBits sampleCount,
			std::initializer_list<const std::string>& filePaths,
			bool hasDepthStencil = true);

		// Create a new descriptor layout
		// Parameters:
		//     device: handle of the VkDevice
		//    shaderModules: vector of shader modules that hold information on shader stages
		void CreateDescriptorSetLayout(VkDevice device, std::vector<std::unique_ptr<D3D::ShaderModuleWrapper>>& shaderModules);

		// Set up vertex input state create info
		// Parameters:
		//     vertexInputStateInfo: a reference to the vertex input state create info to avoid creating a new one in the function
		void SetupVertexInputState(VkPipelineVertexInputStateCreateInfo& vertexInputInfo,
			VkVertexInputBindingDescription* bindingDescription,
			std::vector<VkVertexInputAttributeDescription>& attributeDescriptions);

		// Set up the rasterizer
		// Parameters:
		//     rasterizer: a reference to the rasterization state create info to avoid creating a new one in the the function
		void SetupRasterizer(VkPipelineRasterizationStateCreateInfo& rasterizer);

		// Set the values of the sample state create info
		// Parameters:
		//     multiSampling: a reference to the multisample state create info to avoid creating a new one in the function
		//     sampleCount: the amount of rasterization samples
		void SetMultisampleStateCreateInfo(VkPipelineMultisampleStateCreateInfo& multisampling,
			VkSampleCountFlagBits& sampleCount);
		
		// Set the values of the depth stencil state create info
		// Parameters:
		//     depthStencil: a reference to the septh stencil state create info to avoid creating a new one in the function
		//     hasDepthStencil: boolean that indicates is this pipeline needs a depthStencil or not
		void SetDepthStencilStateCreateInfo(VkPipelineDepthStencilStateCreateInfo& depthStencil,
			bool hasDepthStencil);

		// Set the values of the color blend attachment state
		// Parameters:
		//     colorBlendAttachment: a reference to the color blend attachment state to avoid creating a new one in the function
		void SetColorBlendAttachmentState(VkPipelineColorBlendAttachmentState& colorBlendAttachment);

		// Set color blend state create info
		// Parameters:
		//     colorblendStateCreateInfo: a reference to the color blend state info to avoid creating a new one in the function
		//     colorBlendAttachment: a pointer to the color blend attachment needed in the color blend state create info
		void SetColorblendStateCreateInfo(VkPipelineColorBlendStateCreateInfo& colorBlending,
			VkPipelineColorBlendAttachmentState* colorBlendAttachment);

		// Create pipeline layout create info
		// Parameters:
		//     pipelineLayoutInfo: a reference to the layout create info to avoid creating a new one in the function
		void SetPipelineLayoutCreateInfo(VkPipelineLayoutCreateInfo& pipelineLayoutInfo,
			std::vector<std::unique_ptr<D3D::ShaderModuleWrapper>>& shaderModules,
			std::vector<VkPushConstantRange>& pushConstants);
	};
}

#endif // !PipelineWrapperIncluded