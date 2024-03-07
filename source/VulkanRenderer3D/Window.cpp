#include "Window.h"
#include <functional>

//static D3D::WindowStruct m_Window{};

void D3D::Window::CreateWindow(int width, int height)
{
	m_Window.Width = width;
	m_Window.Height = height;

	// Initialize the window
	InitWindow();
}

D3D::Window::~Window()
{
	// Destroy the window
	glfwDestroyWindow(m_Window.pWindow);

	// Terminate glfw
	glfwTerminate();
}

void D3D::Window::SetFrameBufferResized(bool value)
{
	m_Window.FrameBufferResized = value;
}

void D3D::Window::InitWindow()
{
	//Initialize glfw
	glfwInit();

	//Tell GLFW not to create an OpenGL context
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	//Initialize the window
	m_Window.pWindow = glfwCreateWindow(m_Window.Width, m_Window.Height, "Vulkan", nullptr, nullptr);
	glfwSetWindowUserPointer(m_Window.pWindow, this);
	glfwSetFramebufferSizeCallback(m_Window.pWindow, FramebufferResizeCallback);
}

void D3D::Window::FramebufferResizeCallback(GLFWwindow* pWindow, int width, int height)
{
	// Update values of global window variable after resizing
	auto& window = GetInstance().GetWindowStruct();
	window.FrameBufferResized = true;
	window.pWindow = pWindow;
	window.Width = width;
	window.Height = height;
}
