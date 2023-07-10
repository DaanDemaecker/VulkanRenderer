#include "stdafx.h"

#include "D3DEngine.h"
#include "VulkanRenderer.h"


void load()
{
	//auto& renderer{ D3D::VulkanRenderer::GetInstance() };
}

int main()
{
	const uint32_t windowWidth = 800;
	const uint32_t windowHeight = 600;

	D3D::D3DEngine engine(static_cast<int>(windowWidth), static_cast<int>(windowHeight));
	engine.Run(load);

	return EXIT_SUCCESS;
} 