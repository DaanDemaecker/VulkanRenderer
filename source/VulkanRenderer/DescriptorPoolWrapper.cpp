#include "DescriptorPoolWrapper.h"
#include "VulkanRenderer.h"
#include "Model.h"

D3D::DescriptorPoolWrapper::DescriptorPoolWrapper(uint32_t uboAmount, uint32_t textureAmount)
	:m_UboAmount{uboAmount}, m_TextureAmount{textureAmount}
{
	m_MaxFramesInFlight = VulkanRenderer::GetInstance().GetMaxFrames();

	InitDescriptorPool();
}

void D3D::DescriptorPoolWrapper::Cleanup(VkDevice device)
{
	vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);
	m_pModels.clear();
}

void D3D::DescriptorPoolWrapper::AddModel(Model* pModel)
{
	if (std::find(m_pModels.begin(), m_pModels.end(), pModel) == m_pModels.end())
	{
		m_pModels.push_back(pModel);
	}
}

void D3D::DescriptorPoolWrapper::RemoveModel(Model* pModel)
{
	m_pModels.erase(std::remove(m_pModels.begin(), m_pModels.end(), pModel), m_pModels.end());
}

void D3D::DescriptorPoolWrapper::CreateDescriptorSets(VkDescriptorSetLayout layout, std::vector<VkDescriptorSet>& descriptorSets)
{
	if (m_AllocatedDescriptorSets >= m_MaxDescriptorSets)
	{
		ResizeDescriptorPool();
		return;
	}
	
	m_AllocatedDescriptorSets++;

	auto& renderer{ VulkanRenderer::GetInstance() };

	std::vector<VkDescriptorSetLayout> layouts(m_MaxFramesInFlight, layout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_DescriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(m_MaxFramesInFlight);
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(m_MaxFramesInFlight);
	if (vkAllocateDescriptorSets(renderer.GetDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor sets!");
	}
}

void D3D::DescriptorPoolWrapper::UpdateDescriptorSets(std::vector<std::vector<VkBuffer>>& uboBuffers, std::vector<VkDeviceSize> uboSizes, std::vector<VkDescriptorSet>& descriptorSets)
{
	for (size_t i = 0; i < m_MaxFramesInFlight; i++)
	{
		std::vector<VkWriteDescriptorSet> descriptorWrites(m_UboAmount + m_TextureAmount);

		std::vector<VkDescriptorBufferInfo> bufferInfos{ m_UboAmount };

		for (uint32_t j{ 0 }; j < m_UboAmount; ++j)
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uboBuffers[j][i];

			VkDeviceSize offset{};

			for (uint32_t k{}; k < j; k++)
			{
				offset += uboSizes[k];
			}

			bufferInfo.offset = offset;
			bufferInfo.range = sizeof(uboSizes[j]);

			bufferInfos[j] = bufferInfo;
		}

		for (uint32_t j{ 0 }; j < m_UboAmount; ++j)
		{
			descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[j].dstSet = descriptorSets[i];
			descriptorWrites[j].dstBinding = j;
			descriptorWrites[j].dstArrayElement = 0;
			descriptorWrites[j].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[j].descriptorCount = 1;
			descriptorWrites[j].pBufferInfo = &bufferInfos[j];
		}

		vkUpdateDescriptorSets(VulkanRenderer::GetInstance().GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

void D3D::DescriptorPoolWrapper::UpdateDescriptorSets(std::vector<std::vector<VkBuffer>>& uboBuffers, std::vector<VkDeviceSize> uboSizes, std::vector<VkDescriptorSet>& descriptorSets, std::vector<VkImageView>& imageViews)
{
	auto& renderer{ D3D::VulkanRenderer::GetInstance() };

	for (size_t i = 0; i < m_MaxFramesInFlight; i++)
	{
		std::vector<VkWriteDescriptorSet> descriptorWrites(m_UboAmount + m_TextureAmount);

		std::vector<VkDescriptorBufferInfo> bufferInfos{ m_UboAmount };

		for (uint32_t j{ 0 }; j < m_UboAmount; ++j)
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uboBuffers[j][i];

			VkDeviceSize offset{};

			for (uint32_t k{}; k < j; k++)
			{
				offset += uboSizes[k];
			}

			bufferInfo.offset = offset;
			bufferInfo.range = sizeof(uboSizes[j]);

			bufferInfos[j] = bufferInfo;
		}

		for (uint32_t j{ 0 }; j < m_UboAmount; ++j)
		{
			descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[j].dstSet = descriptorSets[i];
			descriptorWrites[j].dstBinding = j;
			descriptorWrites[j].dstArrayElement = 0;
			descriptorWrites[j].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[j].descriptorCount = 1;
			descriptorWrites[j].pBufferInfo = &bufferInfos[j];
		}

		std::vector<VkDescriptorImageInfo> imageInfos{};
		imageInfos.reserve(m_TextureAmount);

		for (auto& imageView : imageViews)
		{
			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = imageView;
			imageInfo.sampler = renderer.GetSampler();

			imageInfos.push_back(imageInfo);
		}

		

		for (uint32_t j{m_UboAmount}; j < m_UboAmount + m_TextureAmount; ++j)
		{
			descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[j].dstSet = descriptorSets[i];
			descriptorWrites[j].dstBinding = j;
			descriptorWrites[j].dstArrayElement = 0;
			descriptorWrites[j].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[j].descriptorCount = 1;

			descriptorWrites[j].pImageInfo = &imageInfos[j - m_UboAmount];
		}
		vkUpdateDescriptorSets(VulkanRenderer::GetInstance().GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

void D3D::DescriptorPoolWrapper::ResizeDescriptorPool()
{
	vkDestroyDescriptorPool(VulkanRenderer::GetInstance().GetDevice(), m_DescriptorPool, nullptr);

	m_MaxDescriptorSets *= m_IncreaseFactor;
	m_AllocatedDescriptorSets = 0;

	InitDescriptorPool();

	for (auto& pModel : m_pModels)
	{
		pModel->CreateDescriptorSets();
	}
}

void D3D::DescriptorPoolWrapper::InitDescriptorPool()
{
	auto& renderer{ VulkanRenderer::GetInstance() };

	if (m_TextureAmount > 0)
	{
		std::array<VkDescriptorPoolSize, 2> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(m_MaxFramesInFlight * m_MaxDescriptorSets * m_UboAmount);
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(m_MaxFramesInFlight * m_MaxDescriptorSets * m_TextureAmount);

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(m_MaxFramesInFlight * m_MaxDescriptorSets);

		if (vkCreateDescriptorPool(renderer.GetDevice(), &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}
	else
	{
		VkDescriptorPoolSize poolSize{};
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = static_cast<uint32_t>(m_MaxFramesInFlight * m_MaxDescriptorSets * m_UboAmount);

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = static_cast<uint32_t>(m_MaxFramesInFlight * m_MaxDescriptorSets);

		if (vkCreateDescriptorPool(renderer.GetDevice(), &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}
}


