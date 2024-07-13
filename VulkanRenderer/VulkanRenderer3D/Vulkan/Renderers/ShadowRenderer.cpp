// ShadowRenderer.cpp

// File includes
#include "ShadowRenderer.h"
#include "Vulkan/VulkanUtils.h"
#include "Vulkan/Wrappers/PipelineWrapper.h"
#include "Engine/ConfigManager.h"
#include "Vulkan/Wrappers/GPUObject.h"
#include "Vulkan/Wrappers/DescriptorPoolWrapper.h"
#include "DataTypes/DirectionalLightObject.h"
#include "DataTypes/RenderClasses/Model.h"
#include "Vulkan/Wrappers/Viewport.h"

D3D::ShadowRenderer::ShadowRenderer(GPUObject* pGPUObject, VkExtent2D swapchainExtent)
{
	// Set amount of samples
	m_MsaaSamples = VK_SAMPLE_COUNT_1_BIT;

	auto device = pGPUObject->GetDevice();

	CreateDepthImage(pGPUObject, swapchainExtent);

	CreateRenderPass(device);

	m_ShadowTexture.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

	CreateFramebuffers(device, swapchainExtent);

	m_pViewport = std::make_unique<Viewport>();
}

void D3D::ShadowRenderer::Cleanup(VkDevice device)
{
	m_pShadowTextureObject = nullptr;

	m_pShadowPipeline->Cleanup(device);

	vkDestroyRenderPass(device, m_ShadowRenderpass, nullptr);

	vkDestroyFramebuffer(device, m_ShadowFrameBuffer, nullptr);
}

void D3D::ShadowRenderer::CreateDepthImage(GPUObject* pGPUObject, VkExtent2D /*swapchainExtent*/)
{
	auto device{ pGPUObject->GetDevice() };

	// Create VkImage
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = m_ShadowMapSize;
	imageInfo.extent.height = m_ShadowMapSize;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = VulkanUtils::FindDepthFormat(pGPUObject->GetPhysicalDevice());
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.samples = m_MsaaSamples;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	vkCreateImage(device, &imageInfo, nullptr, &m_ShadowTexture.image);
	// Allocate memory and bind image
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, m_ShadowTexture.image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = VulkanUtils::FindMemoryType(pGPUObject->GetPhysicalDevice(), memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	vkAllocateMemory(device, &allocInfo, nullptr, &m_ShadowTexture.imageMemory);
	vkBindImageMemory(device, m_ShadowTexture.image, m_ShadowTexture.imageMemory, 0);

	// Create ImageView for depth attachment
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = m_ShadowTexture.image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = VulkanUtils::FindDepthFormat(pGPUObject->GetPhysicalDevice());
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	vkCreateImageView(device, &viewInfo, nullptr, &m_ShadowTexture.imageView);
}

void D3D::ShadowRenderer::CreateFramebuffers(VkDevice device, VkExtent2D /*swapchainExtent*/)
{
	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = m_ShadowRenderpass; // Render pass created in the next step
	framebufferInfo.attachmentCount = 1;
	framebufferInfo.pAttachments = &m_ShadowTexture.imageView;
	framebufferInfo.width = m_ShadowMapSize;
	framebufferInfo.height = m_ShadowMapSize;
	framebufferInfo.layers = 1;

	vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_ShadowFrameBuffer);
}

void D3D::ShadowRenderer::CreateRenderPass(VkDevice device)
{
	VkAttachmentDescription attachmentDescription = {};
	attachmentDescription.format = VK_FORMAT_D32_SFLOAT;
	attachmentDescription.samples = m_MsaaSamples;
	attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

	VkAttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment = 0;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 0;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	// Use subpass dependencies for layout transitions
	std::array<VkSubpassDependency, 2> dependencies;

	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &attachmentDescription;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
	renderPassInfo.pDependencies = dependencies.data();

	vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_ShadowRenderpass);
}

void D3D::ShadowRenderer::SetupDescriptorSets()
{
	CreateDescriptorSets();

	UpdateDescriptorSets();

	m_DescriptorSetInitialized = true;
}

void D3D::ShadowRenderer::CreateDescriptorSets()
{
	// Get pointer to the descriptorpool wrapper
	auto descriptorPool = m_pShadowPipeline->GetDescriptorPool();
	// Create descriptorpools
	descriptorPool->CreateDescriptorSets(m_pShadowPipeline->GetDescriptorSetLayout(), m_DescriptorSets);
}

void D3D::ShadowRenderer::UpdateDescriptorSets()
{
	// Get pointer to the descriptorpool wrapper
	auto descriptorPool = m_pShadowPipeline->GetDescriptorPool();

	// Create list of descriptor objects and add the objects of the model to it
	std::vector<DescriptorObject*> descriptorObjectList{ VulkanRenderer3D::GetInstance().GetGlobalLight()->GetTransformDescriptorObject() };

	// Update descriptorsets
	descriptorPool->UpdateDescriptorSets(m_DescriptorSets, descriptorObjectList);
}

void D3D::ShadowRenderer::RecreateFrameBuffer()
{
	auto device{ VulkanRenderer3D::GetInstance().GetDevice() };

	vkDeviceWaitIdle(device);


}

void D3D::ShadowRenderer::CreatePipeline(VkDevice device)
{

	auto& configManager{ ConfigManager::GetInstance() };

	std::initializer_list<const std::string> filePaths{ configManager.GetString("ShadowVertName"),
		configManager.GetString("ShadowFragName") };

	m_pShadowPipeline = std::make_unique<D3D::PipelineWrapper>
		(device, m_ShadowRenderpass, m_MsaaSamples, filePaths);


	m_pShadowTextureObject = std::make_unique<TextureDescriptorObject>(m_ShadowTexture);
}

D3D::TextureDescriptorObject* D3D::ShadowRenderer::GetTextureDescriptorObject()
{
	return m_pShadowTextureObject.get();
}

void D3D::ShadowRenderer::Render(std::vector<std::unique_ptr<Model>>& pModels, VkExtent2D /*swapchainExtent*/)
{
	if (!m_DescriptorSetInitialized)
	{
		SetupDescriptorSets();
	}

	// Get reference to renderer
	auto& renderer{ VulkanRenderer3D::GetInstance() };
	// Get index of current frame
	auto frame{ renderer.GetCurrentFrame() };

	auto commandBuffer{ renderer.GetCurrentCommandBuffer() };

	VkExtent2D extent{ m_ShadowMapSize, m_ShadowMapSize };

	m_pViewport->SetViewport(commandBuffer, extent);

	VkClearValue clearValue = {};
	clearValue.depthStencil = { 1.0f, 0 };

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_ShadowRenderpass;
	renderPassInfo.framebuffer = m_ShadowFrameBuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = extent;
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearValue;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	// Bind pipeline and descriptor sets, draw your scene from the light's perspective here
	m_pShadowPipeline->BindPipeline(commandBuffer);

	// Bind descriptor sets
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pShadowPipeline->GetPipelineLayout(), 0, 1, &m_DescriptorSets[frame], 0, nullptr);

	for (auto& model : pModels)
	{
		model->RenderShadow(commandBuffer, m_pShadowPipeline->GetPipelineLayout());
	}

	vkCmdEndRenderPass(commandBuffer);
}