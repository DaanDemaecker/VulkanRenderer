// ShadowRenderer.cpp

// File includes
#include "ShadowRenderer.h"
#include "VulkanUtils.h"
#include "PipelineWrapper.h"
#include "ConfigManager.h"
#include "GPUObject.h"
#include "ImageManager.h"
#include "DescriptorPoolWrapper.h"
#include "DirectionalLightObject.h"
#include "Model.h"

D3D::ShadowRenderer::ShadowRenderer(GPUObject* pGPUObject, VkSampleCountFlagBits msaaSamples,
	D3D::ImageManager* pImageManager, VkCommandBuffer commandBuffer)
{
	// Set amount of samples
	m_MsaaSamples = msaaSamples;

	auto device = pGPUObject->GetDevice();

	CreateRenderPass(device);

	CreateDepthImage(pGPUObject, pImageManager, commandBuffer);

	CreateFramebuffers(device);
}

void D3D::ShadowRenderer::Cleanup(VkDevice device)
{
	m_ShadowTexture.Cleanup(device);

	m_pShadowPipeline->Cleanup(device);

	m_pLightProjectionObject->Cleanup(device);

	vkDestroyRenderPass(device, m_ShadowRenderpass, nullptr);

	vkDestroyFramebuffer(device, m_ShadowFrameBuffer, nullptr);
}

void D3D::ShadowRenderer::CreateDepthImage(GPUObject* pGPUObject, ImageManager* pImageManager, VkCommandBuffer commandBuffer)
{
	// Create depth image trough vulkan utils
	VulkanUtils::CreateDepthImage(m_ShadowTexture, pGPUObject, m_MsaaSamples, VkExtent2D{m_ShadowMapSize, m_ShadowMapSize}, pImageManager, commandBuffer);
}

void D3D::ShadowRenderer::CreateFramebuffers(VkDevice device)
{
	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = m_ShadowRenderpass; // Render pass created in the next step
	framebufferInfo.attachmentCount = 1;
	framebufferInfo.pAttachments = &m_ShadowTexture.imageView;
	//framebufferInfo.width = swapchainExtent.width;
	//framebufferInfo.height = swapchainExtent.height;
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

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &attachmentDescription;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

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
	std::vector<DescriptorObject*> descriptorObjectList{m_pLightProjectionObject.get()};

	// Update descriptorsets
	descriptorPool->UpdateDescriptorSets(m_DescriptorSets, descriptorObjectList);
}

void D3D::ShadowRenderer::CreatePipeline(VkDevice device)
{

	auto& configManager{ ConfigManager::GetInstance() };

	std::initializer_list<const std::string> filePaths{ configManager.GetString("ShadowVertName"),
		configManager.GetString("ShadowFragName") };

	m_pShadowPipeline = std::make_unique<D3D::PipelineWrapper>
		(device, m_ShadowRenderpass, m_MsaaSamples, filePaths);


	m_pLightProjectionObject = std::make_unique<D3D::UboDescriptorObject<glm::mat4>>();
}

void D3D::ShadowRenderer::Render(std::vector<std::unique_ptr<Model>>& pModels)
{	
	if (!m_DescriptorSetInitialized)
	{
		SetupDescriptorSets();
	}

	// Get reference to renderer
	auto& renderer{ VulkanRenderer3D::GetInstance() };
	// Get index of current frame
	auto frame{ renderer.GetCurrentFrame() };

	m_pLightProjectionObject->UpdateUboBuffer(renderer.GetGlobalLight()->GetLightMatrix(), frame);

	auto commandBuffer{renderer.GetCurrentCommandBuffer()};

	VkClearValue clearValue = {};
	clearValue.depthStencil = { 1.0f, 0 };
	
	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_ShadowRenderpass;
	renderPassInfo.framebuffer = m_ShadowFrameBuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = { m_ShadowMapSize, m_ShadowMapSize };
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
