// DescriptorPoolWrapper.cpp

// File includes
#include "DescriptorPoolWrapper.h"
#include "Vulkan/Vulkan3D.h"
#include "DataTypes/RenderClasses/Model.h" 
#include "DataTypes/DescriptorObjects/DescriptorObject.h"
#include "ShaderModuleWrapper.h"


DDM3::DescriptorPoolWrapper::DescriptorPoolWrapper(std::vector<std::unique_ptr<DDM3::ShaderModuleWrapper>>& shaderModules)
{
	// Read the number of bindings per type
	ReadDescriptorTypeCount(shaderModules);

	// Initialize the descriptor pool
	InitDescriptorPool();
}


void DDM3::DescriptorPoolWrapper::Cleanup(VkDevice device)
{
	// Destroy the descriptorPool
	vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);

	// Clear the vector of moddels
	m_pModels.clear();
}

void DDM3::DescriptorPoolWrapper::AddModel(Model* pModel)
{
	// Check if the model is already in the vector, if not add it
	if (std::find(m_pModels.begin(), m_pModels.end(), pModel) == m_pModels.end())
	{
		m_pModels.push_back(pModel);
	}
}

void DDM3::DescriptorPoolWrapper::RemoveModel(Model* pModel)
{
	// Remove model from vector
	m_pModels.erase(std::remove(m_pModels.begin(), m_pModels.end(), pModel), m_pModels.end());
}

void DDM3::DescriptorPoolWrapper::CreateDescriptorSets(VkDescriptorSetLayout layout, std::vector<VkDescriptorSet>& descriptorSets)
{
	// Check if the amount of already allocated descriptorsets is larger or equal to the max amoount, if it is, resize and return
	if (m_AllocatedDescriptorSets >= m_MaxDescriptorSets)
	{
		ResizeDescriptorPool();
		return;
	}

	// Get a reference to the renderer for later use
	auto& renderer{ Vulkan3D::GetInstance().GetRenderer()};

	// Get the amount of frames in flight
	auto maxFrames = Vulkan3D::GetMaxFrames();

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
	if (vkAllocateDescriptorSets(Vulkan3D::GetInstance().GetDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	// Increase the amount of allocated descriptorsets
	m_AllocatedDescriptorSets++;
}

void DDM3::DescriptorPoolWrapper::UpdateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<DescriptorObject*>& descriptorObjects)
{
	// Loop trough all the descriptor sets
	for (int i{}; i < static_cast<int>(descriptorSets.size()); i++)
	{
		// Create a vector of descriptor writes
		std::vector<VkWriteDescriptorSet> descriptorWrites{};

		// Initialize current binding with 0
		int binding{};

		// Loop trough all descriptor objects and add the descriptor writes
		for (auto& descriptorObject : descriptorObjects)
		{
			descriptorObject->AddDescriptorWrite(descriptorSets[i], descriptorWrites, binding, i);
		}


		// Update descriptorsets
		vkUpdateDescriptorSets(Vulkan3D::GetInstance().GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

void DDM3::DescriptorPoolWrapper::ResizeDescriptorPool()
{
	// Destroy current descriptorpool
	vkDestroyDescriptorPool(Vulkan3D::GetInstance().GetDevice(), m_DescriptorPool, nullptr);

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

void DDM3::DescriptorPoolWrapper::ReadDescriptorTypeCount(std::vector<std::unique_ptr<DDM3::ShaderModuleWrapper>>& shaderModules)
{
	// Loop trough all shader modules and add the descriptor count
	for (auto& shaderModule : shaderModules)
	{
		shaderModule->AddDescriptorTypeCount(m_DescriptorTypeCount);
	}
}

void DDM3::DescriptorPoolWrapper::InitDescriptorPool()
{
	// Get a reference to the renderer
	auto& renderer{ Vulkan3D::GetInstance().GetRenderer()};

	// Get the amount of frames in flight
	auto frames{ Vulkan3D::GetMaxFrames() };

	// Create a vector of pool sizes
	std::vector<VkDescriptorPoolSize> poolSizes{};

	// Loop trough all the descriptor types
	for (auto& pair : m_DescriptorTypeCount)
	{
		VkDescriptorPoolSize descriptorPoolSize{};
		// Set the type of the poolsize
		descriptorPoolSize.type = pair.first;
		// Calculate the amount of descriptors needed from this type
		descriptorPoolSize.descriptorCount = static_cast<uint32_t>(pair.second * m_MaxDescriptorSets * frames);
	
		// Add the descriptor poolsize to the vector
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
	if (vkCreateDescriptorPool(Vulkan3D::GetInstance().GetDevice(), &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}


