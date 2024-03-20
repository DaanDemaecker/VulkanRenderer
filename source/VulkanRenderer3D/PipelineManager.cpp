// PipelineManager.cpp

// File includes
#include "PipelineManager.h"
#include "Utils.h"
#include "ConfigManager.h"

D3D::PipelineManager::PipelineManager()
	:m_VertexFunction{ConfigManager::GetInstance().GetString("VertexFunction")},
	m_FragmenFunction{ConfigManager::GetInstance().GetString("FragmentFunction")}
{
}

void D3D::PipelineManager::Cleanup(VkDevice device)
{
	// Loop trough all descriptorsetlayouts
	for (auto& pair : m_DescriptorSetLayouts)
	{
		// Destroy the descriptor set layout
		vkDestroyDescriptorSetLayout(device, pair.second[0], nullptr);
	}

	// Loop trough all the pipelines
	for (auto& pipeline : m_GraphicPipelines)
	{
		// Call cleanup function
		pipeline.second.Cleanup(device);
	}
}

void D3D::PipelineManager::AddDefaultPipeline(VkDevice device, uint32_t maxFrames, VkRenderPass renderPass, VkSampleCountFlagBits sampleCount)
{
	// Get config manager
	auto& configManager{ ConfigManager::GetInstance() };

	// Initialize default pipeline name 
	m_DefaultPipelineName = configManager.GetString("DefaultPipelineName");

	// Add default pipeline
	AddGraphicsPipeline(device, maxFrames, renderPass, sampleCount, m_DefaultPipelineName,
		configManager.GetString("DefaultVertName"),
		configManager.GetString("DefaultFragName"),
		configManager.GetInt("DefaultPipelineVertexUbos"),
		configManager.GetInt("DefaultPipelineFragmentUbos"),
		configManager.GetInt("DefaultPipelineTextures"), false);

}


void D3D::PipelineManager::AddGraphicsPipeline(VkDevice device, uint32_t maxFrames, VkRenderPass renderPass, VkSampleCountFlagBits sampleCount, const std::string& pipelineName, const std::string& vertShaderName, const std::string& fragShaderName, int vertexUbos, int fragmentUbos, int textureAmount, bool isSkybox)
{
	// Check if pipeline already exists, if it does, delete it
	if (m_GraphicPipelines.contains(pipelineName))
	{
		m_GraphicPipelines[pipelineName].Cleanup(device);
	}

	// Read in vertex shader
	auto vertShaderCode = Utils::readFile(vertShaderName);
	// Read in fragment shader
	auto fragShaderCode = Utils::readFile(fragShaderName);

	// Creeate vertex shader module
	VkShaderModule vertShaderModule = CreateShaderModule(device, vertShaderCode);
	// Create fragment shader module
	VkShaderModule fragShaderModule = CreateShaderModule(device, fragShaderCode);

	// Create shader stage create info
	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	// Set type to pipeline shader stage create info
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	// Set stage to vertex
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	// Give the correct shader module
	vertShaderStageInfo.module = vertShaderModule;
	// Give the name of the function
	vertShaderStageInfo.pName = m_VertexFunction.c_str();

	// Create shader stage create info
	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	// Set type to pipeline shader stage create info
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	// Set stage to fragment
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	// Give the correct shader module
	fragShaderStageInfo.module = fragShaderModule;
	// Give the name of the function
	fragShaderStageInfo.pName = m_VertexFunction.c_str();

	// Create array for shaderstages
	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	
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
	SetDepthStencilStateCreateInfo(depthStencil, isSkybox);

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
	// Set pipeline layout info
	SetPipelineLayoutCreateInfo(pipelineLayoutInfo, device, maxFrames, vertexUbos, fragmentUbos, textureAmount);

	// Create pipeline layout
	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &m_GraphicPipelines[pipelineName].pipelineLayout) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to create pipeline layout!");
	}

	// Create pipeline create info
	VkGraphicsPipelineCreateInfo pipelineInfo{};
	// Set type to graphics pipeline create info
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	// Set stage count to 2
	pipelineInfo.stageCount = 2;
	// Give shaderstages
	pipelineInfo.pStages = shaderStages;
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
	pipelineInfo.layout = m_GraphicPipelines[pipelineName].pipelineLayout;
	// Give renderpass
	pipelineInfo.renderPass = renderPass;
	// Set subpass to 0
	pipelineInfo.subpass = 0;
	// Set basepipeline to null handle
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	// Create graphics pipeline
	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicPipelines[pipelineName].pipeline) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	// Delete fragment shader module
	vkDestroyShaderModule(device, fragShaderModule, nullptr);
	// Delete vertex shader module
	vkDestroyShaderModule(device, vertShaderModule, nullptr);
}

D3D::PipelinePair& D3D::PipelineManager::GetPipeline(const std::string& name)
{
	// Check if pipeline exists
	if (m_GraphicPipelines.contains(name))
	{
		// If it exists, return the correct pipeline
		return m_GraphicPipelines[name];
	}
	else
	{
		// If not, return default pipeline
		return m_GraphicPipelines[m_DefaultPipelineName];
	}
}

std::vector<VkDescriptorSetLayout>& D3D::PipelineManager::GetDescriptorSetLayout(VkDevice device, uint32_t maxFrames, int vertexUbos, int fragmentUbos, int textureAmount)
{
	// Create tuple for vertexUbos, fragmentUbos and texture amount to use as key in the map
	std::tuple<int, int, int> tuple{ vertexUbos, fragmentUbos, textureAmount };

	// Check if map contains the requestet layout
	if (!m_DescriptorSetLayouts.contains(tuple))
	{
		// If not, create it
		CreateDescriptorLayout(device, maxFrames, vertexUbos, fragmentUbos, textureAmount);
	}

	// Return the requested layout
	return m_DescriptorSetLayouts[tuple];
}

void D3D::PipelineManager::CreateDescriptorLayout(VkDevice device, uint32_t maxFrames, int vertexUbos, int fragmentUbos, int textureAmount)
{
	// Create vector of descriptorsetlayoutbindings the size of the sum of vertexUbos, fragmentUbos and textureamount;
	std::vector<VkDescriptorSetLayoutBinding> bindings(vertexUbos + fragmentUbos + textureAmount);

	// Loop trough the amount of vertex ubos, starting with 0
	for (int i{}; i < vertexUbos; ++i)
	{
		// Create uboLayoutbinding
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		// Set binding to i
		uboLayoutBinding.binding = i;
		// Set type to uniform buffer
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		// Set descriptorCount to 1
		uboLayoutBinding.descriptorCount = 1;
		// Set stage to vertex stage
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		// Set immutableSampler to nullptr
		uboLayoutBinding.pImmutableSamplers = nullptr;

		// Place uboLayoutbinding in the correct place in the bindings vector
		bindings[i] = uboLayoutBinding;
	}

	// Loop trough the amount of fragment ubos starting at the number of vertex ubos
	for (int i{ vertexUbos }; i < fragmentUbos + vertexUbos; ++i)
	{
		// Create ubolayoutbinding
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		// Set binding to i
		uboLayoutBinding.binding = i;
		// Set type to uniform buffer
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		// Set descriptorcount to 1
		uboLayoutBinding.descriptorCount = 1;
		// Set stage to fragment stage
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		// Set immutable sampler to nullptr
		uboLayoutBinding.pImmutableSamplers = nullptr;

		// Place uboLayoutBinding in the correct place in the bindings vector
		bindings[i] = uboLayoutBinding;
	}

	// Loop trough the amount of textures, starting at the sum of the amount of vertex and fragment ubos
	for (int i{ vertexUbos + fragmentUbos }; i < vertexUbos + fragmentUbos + textureAmount; ++i)
	{
		// Create sampler layout binding
		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		// Set binding to i
		samplerLayoutBinding.binding = i;
		// Set descriptorcount to 1
		samplerLayoutBinding.descriptorCount = 1;
		// Set type to combined image sampler
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		// Set immutable sampler to nullptr
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		// Set stage to fragment stage
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		// Place sampler layout binding in the correct place in the bindings vector
		bindings[i] = samplerLayoutBinding;
	}

	// Create layout info
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	// Set type to descriptor set layout create info
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	// Set bindingcount to the amount of bindings
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	// Set bindings to the data of bindings vector
	layoutInfo.pBindings = bindings.data();

	// Create tuble to use as a key in the descriptorsetlayout map
	std::tuple<int, int, int> tuple{ vertexUbos, fragmentUbos, textureAmount };

	// Create descriptorsetlayout handle, initialize with VK_NULL_HANDLE
	VkDescriptorSetLayout layout = VK_NULL_HANDLE;                                                                                                                                                                                                             

	// Create descriptorset layout
	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &layout) != VK_SUCCESS)
	{
		// If not successfull, throw runtime error
		throw std::runtime_error("failed to create descriptor set layout!");
	}

	// Create a vector of descriptorsets the size of the max amount of frames in flight and place it in the map
	m_DescriptorSetLayouts[tuple] = std::vector<VkDescriptorSetLayout>(maxFrames, layout);
}

VkShaderModule D3D::PipelineManager::CreateShaderModule(VkDevice device, const std::vector<char>& code)
{
	// Create modlue create info
	VkShaderModuleCreateInfo createInfo{};
	// Set type to shader module craete info
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	// Set the code size to the size of the vector
	createInfo.codeSize = code.size();
	// Give the data of the vector as a pointer
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	// Create shadermodule
	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		// Unsuccessful, throw runtime error
		throw std::runtime_error("failed to create shader module!");
	}

	// Return the shader module
	return shaderModule;
}

void D3D::PipelineManager::SetupVertexInputState(VkPipelineVertexInputStateCreateInfo& vertexInputInfo, VkVertexInputBindingDescription* bindingDescription,
	std::vector<VkVertexInputAttributeDescription>& attributeDescriptions)
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

void D3D::PipelineManager::SetupRasterizer(VkPipelineRasterizationStateCreateInfo& rasterizer)
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
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	// Disable depth bias
	rasterizer.depthBiasEnable = VK_FALSE;
	// Set depth bias constant factor to 0
	rasterizer.depthBiasConstantFactor = 0.0f; 
	// Set depth bias clamp to 0
	rasterizer.depthBiasClamp = 0.0f; 
	// Set depth bias slope factor to 0
	rasterizer.depthBiasSlopeFactor = 0.0f; 
}

void D3D::PipelineManager::SetMultisampleStateCreateInfo(VkPipelineMultisampleStateCreateInfo& multisampling, VkSampleCountFlagBits& sampleCount)
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

void D3D::PipelineManager::SetDepthStencilStateCreateInfo(VkPipelineDepthStencilStateCreateInfo& depthStencil, bool isSkybox)
{
	// Set type to depth stencil state create info
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

	// Check if this depth stencil state if for the skybox
	if (isSkybox)
	{
		// If it is:
		// Set depth test enable to false
		depthStencil.depthTestEnable = VK_FALSE;
		// Set depth write enable to false
		depthStencil.depthWriteEnable = VK_FALSE;
	}
	else
	{
		// If it is not:
		// Set depth test enable to true
		depthStencil.depthTestEnable = VK_TRUE;
		// Set depth write enable to true
		depthStencil.depthWriteEnable = VK_TRUE;
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

void D3D::PipelineManager::SetColorBlendAttachmentState(VkPipelineColorBlendAttachmentState& colorBlendAttachment)
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

void D3D::PipelineManager::SetColorblendStateCreateInfo(VkPipelineColorBlendStateCreateInfo& colorBlending, VkPipelineColorBlendAttachmentState* colorBlendAttachment)
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

VkPipelineLayoutCreateInfo D3D::PipelineManager::SetPipelineLayoutCreateInfo(VkPipelineLayoutCreateInfo& pipelineLayoutInfo, VkDevice device, uint32_t maxFrames, int vertexUbos, int fragmentUbos, int textureAmount)
{
	// Set type to pipeline layout create info
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	// Set layoutcount to 1
	pipelineLayoutInfo.setLayoutCount = 1;
	// Get the correct layout
	pipelineLayoutInfo.pSetLayouts = &GetDescriptorSetLayout(device, maxFrames, vertexUbos, fragmentUbos, textureAmount)[0];

	// Code for adding push constant
	/*VkPushConstantRange pushConstantRange = {};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(LightObject);*/

	//pipelineLayoutInfo.pushConstantRangeCount = 1; // Number of push constant ranges used by the pipeline
	//pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange; // Array of push constant ranges used by the pipeline

	return pipelineLayoutInfo;
}