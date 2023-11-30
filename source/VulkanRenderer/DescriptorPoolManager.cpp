#include "DescriptorPoolManager.h"
#include "DescriptorPoolWrapper.h"

void D3D::DescriptorPoolManager::Cleanup(VkDevice device)
{
    for (auto& pool : m_pDescriptorPools)
    {
        pool.second->Cleanup(device);
    }
}

D3D::DescriptorPoolWrapper* D3D::DescriptorPoolManager::GetDescriptorPool(uint32_t uboAmount, uint32_t textureAmount)
{
    m_Pair.first = uboAmount;
    m_Pair.second = textureAmount;

    if(!m_pDescriptorPools.contains(m_Pair))
    {
        m_pDescriptorPools[m_Pair] = std::make_unique<DescriptorPoolWrapper>(uboAmount, textureAmount);
    }

    return m_pDescriptorPools[m_Pair].get();
}
