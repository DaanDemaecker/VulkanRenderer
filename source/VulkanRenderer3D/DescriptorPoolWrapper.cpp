// DescriptorPoolWrapper.cpp

// File includes
#include "DescriptorPoolWrapper.h"
#include "VulkanRenderer3D.h"
#include "Model.h"

D3D::DescriptorPoolWrapper::DescriptorPoolWrapper(uint32_t uboAmount, uint32_t textureAmount)
	//Initialize amount of ubos and textures
	:m_UboAmount{uboAmount}, m_TextureAmount{textureAmount}
{
	// Initialize the descriptorPool
	InitDescriptorPool();
}

void D3D::DescriptorPoolWrapper::Cleanup(VkDevice device)
{
	// Destroy the descriptorPool
	vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);

	// Clear the vector of moddels
	m_pModels.clear();
}

void D3D::DescriptorPoolWrapper::AddModel(Model* pModel)
{
	// Check if the model is already in the vector, if not add it
	if (std::find(m_pModels.begin(), m_pModels.end(), pModel) == m_pModels.end())
	{
		m_pModels.push_back(pModel);
	}
}

void D3D::DescriptorPoolWrapper::RemoveModel(Model* pModel)
{
	// Remove model from vector
	m_pModels.erase(std::remove(m_pModels.begin(), m_pModels.end(), pModel), m_pModels.end());
}

void D3D::DescriptorPoolWrapper::CreateDescriptorSets(std::vector<VkDescriptorSetLayout>& layouts, std::vector<VkDescriptorSet>& descriptorSets)
{
	// Check if the amount of already allocated descriptorsets is larger or equal to the max amoount, if it is, resize and return
	if (m_AllocatedDescriptorSets >= m_MaxDescriptorSets)
	{
		ResizeDescriptorPool();
		return;
	}

	// Get a reference to the renderer for later use
	auto& renderer{ VulkanRenderer::GetInstance() };

	// Get the amount of frames in flight
	auto maxFrames = renderer.GetMaxFrames();

	// Create the allocation info for the descriptorsets
	VkDescriptorSetAllocateInfo allocInfo{};
	// Set type to descriptor set allocate info
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	// Give the right descriptorpool
	allocInfo.descriptorPool = m_DescriptorPool;
	// Give the amount of descriptorsets to be allocated
	allocInfo.descriptorSetCount = static_cast<uint32_t>(maxFrames);
	// Give the layout of the descriptorsets
	allocInfo.pSetLayouts = layouts.data();

	// Resize descriptorsets to right amount
	descriptorSets.resize(maxFrames);

	// Allocate descriptorsets, if not succeeded, throw runtime error
	if (vkAllocateDescriptorSets(renderer.GetDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	// Increase the amount of allocated descriptorsets
	m_AllocatedDescriptorSets++;
}

void D3D::DescriptorPoolWrapper::UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<std::vector<VkBuffer>>& uboBuffers, std::vector<VkDeviceSize>& uboSizes,  VkImageView* imageViews)
{
	// Get reference to renderer for later use
	auto& renderer{ D3D::VulkanRenderer::GetInstance() };

	// Loop for the amount off frames there are
	for (size_t i = 0; i < renderer.GetMaxFrames(); i++)
	{
		// Create vector of descriptorwrites the size of the uniform buffers and texture amount combined
		std::vector<VkWriteDescriptorSet> descriptorWrites(m_UboAmount + m_TextureAmount);

		// Create vector for the buffer infos
		std::vector<VkDescriptorBufferInfo> bufferInfos{ m_UboAmount };

		// Loop for the amount off uniform buffers needed
		for (uint32_t j{ 0 }; j < m_UboAmount; ++j)
		{
			// BufferInfos
			// Set the correct buffer
			bufferInfos[j].buffer = uboBuffers[j][i];
			// Offset should be 0
			bufferInfos[j].offset = 0;
			// Give the correct size of the buffer object
			bufferInfos[j].range = uboSizes[j];

			// DescriptorWrites
			// Set the type to WriteDescriptorSet
			descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			// Give the correct descriptorset
			descriptorWrites[j].dstSet = descriptorSets[i];
			// Set the binding used in the shader
			descriptorWrites[j].dstBinding = j;
			// Set array index
			descriptorWrites[j].dstArrayElement = 0;
			// Set descriptor type
			descriptorWrites[j].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			// Set descriptor amount
			descriptorWrites[j].descriptorCount = 1;
			// Set the correct bufferInfo
			descriptorWrites[j].pBufferInfo = &bufferInfos[j];
		}

		// Create vector for imageInfos
		std::vector<VkDescriptorImageInfo> imageInfos{ m_TextureAmount };

		// Loop for the amount of imageInfos there are+
		for (size_t j{}; j < m_TextureAmount; j++)
		{
			// Set imageLayout to read only 
			imageInfos[j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			// Give correct image views
			imageInfos[j].imageView = *(imageViews + j);
			// Set sampler
			imageInfos[j].sampler = renderer.GetSampler();
		}

		// Loop for the amount of textures, starting from the amount of uniform buffers
		for (uint32_t j{ m_UboAmount }; j < m_UboAmount + m_TextureAmount; ++j)
		{
			// Set type to write descriptor set
			descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			// Give correct descriptorSet
			descriptorWrites[j].dstSet = descriptorSets[i];
			// Set binding used in shader
			descriptorWrites[j].dstBinding = j;
			// Set array index
			descriptorWrites[j].dstArrayElement = 0;
			// Set descriptor type to combined image sampler
			descriptorWrites[j].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			// Set descriptor amount
			descriptorWrites[j].descriptorCount = 1;
			// Give correct imageInfo
			descriptorWrites[j].pImageInfo = &imageInfos[j - m_UboAmount];
		}

		// Update descriptorsets
		vkUpdateDescriptorSets(VulkanRenderer::GetInstance().GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

void D3D::DescriptorPoolWrapper::ResizeDescriptorPool()
{
	// Destroy current descriptorpool
	vkDestroyDescriptorPool(VulkanRenderer::GetInstance().GetDevice(), m_DescriptorPool, nullptr);

	// Multiply max amount of descriptorsets by increaseFactor
	m_MaxDescriptorSets *= m_IncreaseFactor;
	// Reset amount of allocated descriptorsets to 0
	m_AllocatedDescriptorSets = 0;

	// Initialize the descriptorPool again
	InitDescriptorPool();

	// Create new descriptorsets for all models boudn to this descriptorpool
	for (auto& pModel : m_pModels)
	{
		pModel->CreateDescriptorSets();
	}
}

void D3D::DescriptorPoolWrapper::InitDescriptorPool()
{
	// Get reference to renderer for later use
	auto& renderer{ VulkanRenderer::GetInstance() };
	// Get amount of frames
	auto maxFrames = renderer.GetMaxFrames();

	// Check if textures need to be allocated in descriptorpool
	if (m_TextureAmount > 0)
	{
		// Create array for poolsizes for buffers and textures
		std::array<VkDescriptorPoolSize, 2> poolSizes{};
		// Set type of first poolsize to Uniform Buffer
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		// Set count of descriptors to ubo amount * max amount of descriptorsets * amount of frames
		poolSizes[0].descriptorCount = static_cast<uint32_t>(maxFrames * m_MaxDescriptorSets * m_UboAmount);
		// Set type for second poolsize to combined image sampler
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		// Set count of descriptors to texture amount * max amount of descriptorsets * amount of frames
		poolSizes[1].descriptorCount = static_cast<uint32_t>(maxFrames * m_MaxDescriptorSets * m_TextureAmount);

		// Create pool info
		VkDescriptorPoolCreateInfo poolInfo{};
		// Set type to descriptor pool create info
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		// Set sizecount to the size of poolsizes
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		// Give poolSizes
		poolInfo.pPoolSizes = poolSizes.data();
		// Give max sets
		poolInfo.maxSets = static_cast<uint32_t>(maxFrames * m_MaxDescriptorSets);

		// Created descriptorpool, if not successful, throw runtime error
		if (vkCreateDescriptorPool(renderer.GetDevice(), &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}
	else
	{
		// Create poolsize
		VkDescriptorPoolSize poolSize{};
		// Set type to uniform buffer
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		// Set count of descriptors to ubo amount * max amount of descriptorsets * amount of frames
		poolSize.descriptorCount = static_cast<uint32_t>(maxFrames * m_MaxDescriptorSets * m_UboAmount);

		// Create poolinfo
		VkDescriptorPoolCreateInfo poolInfo{};
		// Set type to descriptor pool create info
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		// Set sizeCount to 1
		poolInfo.poolSizeCount = 1;
		// Give poolsize
		poolInfo.pPoolSizes = &poolSize;
		// Give max sets
		poolInfo.maxSets = static_cast<uint32_t>(maxFrames * m_MaxDescriptorSets);

		// Created descriptorpool, if not successful, throw runtime error
		if (vkCreateDescriptorPool(renderer.GetDevice(), &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}
}


