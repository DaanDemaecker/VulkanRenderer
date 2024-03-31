// DescriptorPoolWrapper.cpp

// File includes
#include "DescriptorPoolWrapper.h"
#include "VulkanRenderer3D.h"
#include "Model.h" 
#include "DescriptorObject.h"


D3D::DescriptorPoolWrapper::DescriptorPoolWrapper(std::vector<std::unique_ptr<D3D::ShaderModuleWrapper>>& shaderModules)
{
	ReadDescriptorTypeCount(shaderModules);

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

void D3D::DescriptorPoolWrapper::CreateDescriptorSets(VkDescriptorSetLayout layout, std::vector<VkDescriptorSet>& descriptorSets)
{
	// Check if the amount of already allocated descriptorsets is larger or equal to the max amoount, if it is, resize and return
	if (m_AllocatedDescriptorSets >= m_MaxDescriptorSets)
	{
		ResizeDescriptorPool();
		return;
	}

	// Get a reference to the renderer for later use
	auto& renderer{ VulkanRenderer3D::GetInstance() };

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
	// Create vector of descriptorsets the size of maxFrames and fill with layout
	std::vector<VkDescriptorSetLayout> layouts(maxFrames, layout);
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

void D3D::DescriptorPoolWrapper::UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects)
{
	// Get reference
	for (int i{}; i < descriptorSets.size(); i++)
	{
		std::vector<VkWriteDescriptorSet> descriptorWrites{};

		int binding{};

		for (auto& descriptorObject : descriptorObjects)
		{
			descriptorObject->AddDescriptorWrite(descriptorSets[i], descriptorWrites, binding, i);
		}


		// Update descriptorsets
		vkUpdateDescriptorSets(VulkanRenderer3D::GetInstance().GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

void D3D::DescriptorPoolWrapper::ResizeDescriptorPool()
{
	// Destroy current descriptorpool
	vkDestroyDescriptorPool(VulkanRenderer3D::GetInstance().GetDevice(), m_DescriptorPool, nullptr);

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

void D3D::DescriptorPoolWrapper::ReadDescriptorTypeCount(std::vector<std::unique_ptr<D3D::ShaderModuleWrapper>>& shaderModules)
{
	for (auto& shaderModule : shaderModules)
	{
		shaderModule->AddDescriptorTypeCount(m_DescriptorTypeCount);
	}
}

void D3D::DescriptorPoolWrapper::InitDescriptorPool()
{
	auto& renderer{ VulkanRenderer3D::GetInstance() };

	auto frames{ renderer.GetMaxFrames() };

	std::vector<VkDescriptorPoolSize> poolSizes{};

	for (auto& pair : m_DescriptorTypeCount)
	{
		VkDescriptorPoolSize descriptorPoolSize{};
		descriptorPoolSize.type = pair.first;
		descriptorPoolSize.descriptorCount = static_cast<uint32_t>(pair.second * m_MaxDescriptorSets * frames);
	
		poolSizes.push_back(descriptorPoolSize);
	}

	// Create pool info
	VkDescriptorPoolCreateInfo poolInfo{};
	// Set type to descriptor pool create info
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	// Set sizecount to the size of poolsizes
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	// Give poolSizes
	poolInfo.pPoolSizes = poolSizes.data();
	// Give max sets
	poolInfo.maxSets = static_cast<uint32_t>(frames * m_MaxDescriptorSets);

	// Created descriptorpool, if not successful, throw runtime error
	if (vkCreateDescriptorPool(renderer.GetDevice(), &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}


