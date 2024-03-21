// Window.cpp

// File includes
#include "Window.h"
#include "ConfigManager.h"

// Static library includes
#include <functional>

D3D::Window::Window()
{
	// Set the width of the window
	m_Window.Width = ConfigManager::GetInstance().GetInt("WindowWidth");
	// Set the height of the window
	m_Window.Height = ConfigManager::GetInstance().GetInt("WindowHeight");

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
	// Set the value of frameBufferResized to the requested value
	m_Window.FrameBufferResized = value;
}

void D3D::Window::InitWindow()
{
	glfwInit();

	//Tell GLFW not to create an OpenGL context
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);



	//Initialize the window
	m_Window.pWindow = glfwCreateWindow(m_Window.Width, m_Window.Height, ConfigManager::GetInstance().GetString("WindowName").c_str(),
		nullptr, nullptr);
	glfwSetWindowUserPointer(m_Window.pWindow, this);
	glfwSetFramebufferSizeCallback(m_Window.pWindow, FramebufferResizeCallback);
}

void D3D::Window::FramebufferResizeCallback(GLFWwindow* pWindow, int width, int height)
{
	// Get a reference to the window struct
	auto& window = GetInstance().GetWindowStruct();

	// Update values of global window variable after resizing
	window.FrameBufferResized = true;
	window.pWindow = pWindow;
	window.Width = width;
	window.Height = height;
}