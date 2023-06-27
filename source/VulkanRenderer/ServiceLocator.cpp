#include "stdafx.h"
#include "ServiceLocator.h"

namespace D3D
{
    std::unique_ptr<DefaultRenderer> ServiceLocator::m_pDefaultRenderer{std::make_unique<DefaultRenderer>()};
    std::unique_ptr<Renderer> ServiceLocator::m_pRenderer{ nullptr };

    Renderer& ServiceLocator::GetRenderer()
    {
        return m_pRenderer.get() == nullptr ? *m_pDefaultRenderer.get() : *m_pRenderer.get();
    }

    void ServiceLocator::RegisterRenderer(std::unique_ptr<Renderer> renderer)
    {
        if (renderer != nullptr)
        {
            m_pRenderer = std::move(renderer);
        }
    }
}