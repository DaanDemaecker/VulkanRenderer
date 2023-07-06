#include "stdafx.h"
#include "D3DEngine.h"
#include "VulkanRenderer.h"

D3D::Window g_pWindow{};

D3D::D3DEngine::D3DEngine(int width, int height)
{
	g_pWindow.Width = width;
	g_pWindow.Height = height;
	InitWindow();
}

D3D::D3DEngine::~D3DEngine()
{
	glfwDestroyWindow(g_pWindow.pWindow);
	glfwTerminate();
}

void D3D::D3DEngine::Run(const std::function<void()>& load)
{
	load();

	auto& renderer{ VulkanRenderer::GetInstance() };

	renderer.LoadModel();

	bool shouldQuit{false};

	while (!shouldQuit)
	{
		renderer.Render();

		shouldQuit = glfwWindowShouldClose(g_pWindow.pWindow);
	}
}

void D3D::D3DEngine::InitWindow()
{
	//Initialize glfw
	glfwInit();

	//Tell GLFW not to create an OpenGL context
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	//Initialize the window
	g_pWindow.pWindow = glfwCreateWindow(g_pWindow.Width, g_pWindow.Height, "Vulkan", nullptr, nullptr);
	glfwSetWindowUserPointer(g_pWindow.pWindow, this);
	//glfwSetFramebufferSizeCallback(g_pWindow.pWindow, framebufferResizeCallback);
}
