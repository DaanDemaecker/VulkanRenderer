// PipelineWrapper.cpp

// File includes
#include "PipelineWrapper.h"
#include "Vulkan/Vulkan3D.h"
#include "ShaderModuleWrapper.h"
#include "DescriptorPoolWrapper.h"

// Standard library include
#include <stdexcept>

DDM3::PipelineWrapper::PipelineWrapper(VkDevice device, VkRenderPass renderPass,
	VkSampleCountFlagBits sampleCount,
	std::initializer_list<const std::string>& filePaths, bool hasDepthStencil)
{
	// Create the pipeline
	CreatePipeline(device, renderPass, sampleCount, filePaths, hasDepthStencil);
}

DDM3::PipelineWrapper::~PipelineWrapper()
{
	Cleanup(Vulkan3D::GetInstance().GetDevice());
}

void DDM3::PipelineWrapper::Cleanup(VkDevice device)
{
	// Clean up the descriptor pool
	m_pDescriptorPool->Cleanup(device);
	// Destroy the pipeline
	vkDestroyPipeline(device, m_Pipeline, nullptr);
	//Destroy the pipeline layout
	vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr);
	// Destroy the descriptor layout
	vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);
}

void DDM3::PipelineWrapper::BindPipeline(VkCommandBuffer commandBuffer)
{
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
}

DDM3::DescriptorPoolWrapper* DDM3::PipelineWrapper::GetDescriptorPool()
{
	// Get descriptorpoolwrapper
	return m_pDescriptorPool.get();
}

void DDM3::PipelineWrapper::CreatePipeline(VkDevice device, VkRenderPass renderPass,
	VkSampleCountFlagBits sampleCount,
	std::initializer_list<const std::string>& filePaths, bool hasDepthStencil)
{
	// Create a vector of shader modules the size of the filepaths list
	std::vector<std::unique_ptr<DDM3::ShaderModuleWrapper>> shaderModuleWrappers(filePaths.size());

	int index{};
	
	// Loop trough the file paths and create a shader module for it
	for (auto& filePath : filePaths)
	{
		shaderModuleWrappers[index] = std::make_unique<DDM3::ShaderModuleWrapper>(device, filePath);

		index++;
	}

	// Create hte descriptor set layout
	CreateDescriptorSetLayout(device, shaderModuleWrappers);

	// Create the descriptor pool
	m_pDescriptorPool = std::make_unique<DescriptorPoolWrapper>(shaderModuleWrappers);

	// Create a vector of shader stages the size of shader module wrappers
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages(shaderModuleWrappers.size());

	// Set the shader stages for all the shader modules
	for (size_t i{}; i < shaderStages.size(); i++)
	{
		shaderStages[i] = shaderModuleWrappers[i]->GetShaderStageCreateInfo();
	}

	// Get the binding description for the vertex
	auto bindingDescription = Vertex::getBindingDescription();
	// Get the attribute description for the vertex
	auto attributeDescription = Vertex::getAttributeDescription();

	// Create vertex input info
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	// Setup vertex input info
	SetupVertexInputState(vertexInputInfo, &bindingDescription, attributeDescription);

	// Create input assembly state create info
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	// Set type to pipeline input assembly state create info
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	// Set topology to triangle list
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	// Set primitive restart enable to false
	inputAssembly.primitiveRestartEnable = VK_FALSE;


	// Create dynamic states
	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	// Create dynamic state create info
	VkPipelineDynamicStateCreateInfo dynamicState{};
	// Set type to dynamic state create info
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	// Set state count to size of dynamic states
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	// Give data of dynamic states
	dynamicState.pDynamicStates = dynamicStates.data();

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	// Create rasterization state create info
	VkPipelineRasterizationStateCreateInfo rasterizer{};
	// Set up rasterization state create info
	SetupRasterizer(rasterizer);

	// Create multisample state create info
	VkPipelineMultisampleStateCreateInfo multisampling{};
	// Set multisample state create info
	SetMultisampleStateCreateInfo(multisampling, sampleCount);

	// Create depth stencil state create info
	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	// Set depth stencil state create info
	SetDepthStencilStateCreateInfo(depthStencil, hasDepthStencil);

	// Create color blend attachment state
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	// Set color blend attachment state
	SetColorBlendAttachmentState(colorBlendAttachment);

	// Create color blending create info
	VkPipelineColorBlendStateCreateInfo colorBlending{};
	// Set color blend state create info
	SetColorblendStateCreateInfo(colorBlending, &colorBlendAttachment);

	// Create pipeline layout info
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	// Create vector of pushconstant ranges and add them by looping trough the shader modules
	std::vector<VkPushConstantRange> pushConstants{};
	// Set pipeline layout info
	SetPipelineLayoutCreateInfo(pipelineLayoutInfo, shaderModuleWrappers, pushConstants);

	// Create pipeline layout
	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to create pipeline layout!");
	}

	// Create pipeline create info
	VkGraphicsPipelineCreateInfo pipelineInfo{};
	// Set type to graphics pipeline create info
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	// Set stage count to 2
	pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
	// Give shaderstages
	pipelineInfo.pStages = shaderStages.data();
	// Give vertex input info
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	// Give input assembly
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	// Give viewport state
	pipelineInfo.pViewportState = &viewportState;
	// Give rasterizer
	pipelineInfo.pRasterizationState = &rasterizer;
	// Give sample state
	pipelineInfo.pMultisampleState = &multisampling;
	// Give color blend state
	pipelineInfo.pColorBlendState = &colorBlending;
	// Give dynamic state
	pipelineInfo.pDynamicState = &dynamicState;
	// Give depth stencil state
	pipelineInfo.pDepthStencilState = &depthStencil;
	// Give pipeline layout
	pipelineInfo.layout = m_PipelineLayout;
	// Give renderpass
	pipelineInfo.renderPass = renderPass;
	// Set subpass to 0
	pipelineInfo.subpass = 0;
	// Set basepipeline to null handle
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	// Create graphics pipeline
	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	// Delete all shader modules
	for (auto& shaderModule : shaderModuleWrappers)
	{
		shaderModule->Cleanup(device);
	}

}

void DDM3::PipelineWrapper::CreateDescriptorSetLayout(VkDevice device, std::vector<std::unique_ptr<DDM3::ShaderModuleWrapper>>& shaderModules)
{
	// Create vector of descriptorsetlayoutbindings the size of the sum of vertexUbos, fragmentUbos and textureamount;
	std::vector<VkDescriptorSetLayoutBinding> bindings{};

	// Add the descriptor layout bindings for each shader module
	for (auto& module : shaderModules)
	{
		module->AddDescriptorSetLayoutBindings(bindings);
	}

	// Create layout info
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	// Set type to descriptor set layout create info
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	// Set bindingcount to the amount of bindings
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	// Set bindings to the data of bindings vector
	layoutInfo.pBindings = bindings.data();

	// Create descriptorset layout
	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS)
	{
		// If not successfull, throw runtime error
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void DDM3::PipelineWrapper::SetupVertexInputState(VkPipelineVertexInputStateCreateInfo& vertexInputInfo, VkVertexInputBindingDescription* bindingDescription, std::vector<VkVertexInputAttributeDescription>& attributeDescriptions)
{
	// Set type to pipeline vertex input state create info
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	// Set binding descriptorcount to 1
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	// Give the correct binding description
	vertexInputInfo.pVertexBindingDescriptions = bindingDescription;
	// Set binding of attribute description count to the size of the attributeDescription array
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	// Give the data of the attributeDescriptions arry
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
}

void DDM3::PipelineWrapper::SetupRasterizer(VkPipelineRasterizationStateCreateInfo& rasterizer)
{
	// Set type to rasterization state create info
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	// Enable depthclamp
	rasterizer.depthClampEnable = VK_FALSE;
	// Disable discard
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	// Set polygon mode to fill
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	// Set line width to 1
	rasterizer.lineWidth = 1.0f;
	// Set cullmode to none
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	// Set front face to counter clockwise
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	// Disable depth bias
	rasterizer.depthBiasEnable = VK_FALSE;
	// Set depth bias constant factor to 0
	rasterizer.depthBiasConstantFactor = 0.0f;
	// Set depth bias clamp to 0
	rasterizer.depthBiasClamp = 0.0f;
	// Set depth bias slope factor to 0
	rasterizer.depthBiasSlopeFactor = 0.0f;
}

void DDM3::PipelineWrapper::SetMultisampleStateCreateInfo(VkPipelineMultisampleStateCreateInfo& multisampling, VkSampleCountFlagBits& sampleCount)
{
	// Set type to multisample state create info
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	// Set sample shading to enabled
	multisampling.sampleShadingEnable = VK_TRUE;
	// Give samplecount
	multisampling.rasterizationSamples = sampleCount;
	// Set minimum sample shading to 0.2
	multisampling.minSampleShading = .2f;
	// Set samplemask to nullptr
	multisampling.pSampleMask = nullptr;
	// Set alpha to coverage to disabled
	multisampling.alphaToCoverageEnable = VK_FALSE;
	// Set alpha to one to disabled
	multisampling.alphaToOneEnable = VK_FALSE;
}

void DDM3::PipelineWrapper::SetDepthStencilStateCreateInfo(VkPipelineDepthStencilStateCreateInfo& depthStencil,
	bool hasDepthStencil)
{
	// Set type to depth stencil state create info
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	
	// Check if this pipeline needs the depth stencil
	if (hasDepthStencil)
	{
		// If it is not:
		// Set depth test enable to true
		depthStencil.depthTestEnable = VK_TRUE;
		// Set depth write enable to true
		depthStencil.depthWriteEnable = VK_TRUE;
	}
	else
	{
		// If it is:
		// Set depth test enable to false
		depthStencil.depthTestEnable = VK_FALSE;
		// Set depth write enable to false
		depthStencil.depthWriteEnable = VK_FALSE;
	}

	// Set compare op to compare op less
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	// Set depth bounds test enable to false
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	// Set min depth bounds to 0
	depthStencil.minDepthBounds = 0.0f;
	// Set max depth bounds to 1
	depthStencil.maxDepthBounds = 1.0f;
	// Set stencil test enable to false
	depthStencil.stencilTestEnable = VK_FALSE;
	// Set depth stencil front to empty
	depthStencil.front = {};
	// Set depth stencil back to empty
	depthStencil.back = {};
}

void DDM3::PipelineWrapper::SetColorBlendAttachmentState(VkPipelineColorBlendAttachmentState& colorBlendAttachment)
{
	// Set color write mask to rgba
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	// Enable blending
	colorBlendAttachment.blendEnable = VK_TRUE;
	// Set color blend factor to source alpha
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	// Set color blend destination to one minus source alpha
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	// Set color blend op to add
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	// Set source alpha blend factor to one
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	// Set destiny alpha blend factor to zero
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	// Set alpha blend op to add
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
}

void DDM3::PipelineWrapper::SetColorblendStateCreateInfo(VkPipelineColorBlendStateCreateInfo& colorBlending, VkPipelineColorBlendAttachmentState* colorBlendAttachment)
{
	// Set type to pipeline color blend state create info
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	// Set logic op to false
	colorBlending.logicOpEnable = VK_FALSE;
	// Set logic op to copy
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	// Set attachment count to 1
	colorBlending.attachmentCount = 1;
	// Give pointer to color blend attachment
	colorBlending.pAttachments = colorBlendAttachment;
	// Set all blend constants to 0
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;
}

void DDM3::PipelineWrapper::SetPipelineLayoutCreateInfo(VkPipelineLayoutCreateInfo& pipelineLayoutInfo,
	std::vector<std::unique_ptr<DDM3::ShaderModuleWrapper>>& shaderModules,
	std::vector<VkPushConstantRange>& pushConstants)
{
	// Set type to pipeline layout create info
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	// Set layoutcount to 1
	pipelineLayoutInfo.setLayoutCount = 1;
	// Get the correct layout
	pipelineLayoutInfo.pSetLayouts = &m_DescriptorSetLayout;

	
	for (auto& module : shaderModules)
	{
		module->AddPushConstants(pushConstants);
	}

	// Number of push constant ranges used by the pipeline
	pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstants.size());
	// Array of push constant ranges used by the pipeline
	pipelineLayoutInfo.pPushConstantRanges = pushConstants.data();
}