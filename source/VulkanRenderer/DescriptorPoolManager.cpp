// DescriptorPoolManager.cpp

// File includes
#include "DescriptorPoolManager.h"
#include "DescriptorPoolWrapper.h"

void D3D::DescriptorPoolManager::Cleanup(VkDevice device)
{
    // Loop trough all descriptorpool wrappers
    for (auto& pool : m_pDescriptorPools)
    {
        // Call cleanup on every pool
        pool.second->Cleanup(device);
    }
}

D3D::DescriptorPoolWrapper* D3D::DescriptorPoolManager::GetDescriptorPool(uint32_t uboAmount, uint32_t textureAmount)
{
    // Set ubo amount for pair
    m_Pair.first = uboAmount;

    // Set texture amount for pair
    m_Pair.second = textureAmount;

    // Check if requested descriptorpool already exists, if not create
    if(!m_pDescriptorPools.contains(m_Pair))
    {
        m_pDescriptorPools[m_Pair] = std::make_unique<DescriptorPoolWrapper>(uboAmount, textureAmount);
    }

    // Return requested descriptorpool
    return m_pDescriptorPools[m_Pair].get();
}
