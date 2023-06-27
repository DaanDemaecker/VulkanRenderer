#include "stdafx.h"
#include "D3DEngine.h"

D3D::D3DEngine::D3DEngine()
{
}

D3D::D3DEngine::~D3DEngine()
{
}

void D3D::D3DEngine::Run(const std::function<void()>& load)
{
	load();
}
