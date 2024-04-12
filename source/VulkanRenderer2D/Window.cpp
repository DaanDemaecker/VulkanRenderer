// Window.cpp

// File includes
#include "Window.h"
#include "ConfigManager.h"

// Standard library includes
#include <iostream>

D2D::Window::Window()
{
	// Read window width from config file
	m_Window.Width = ConfigManager::GetInstance().GetInt("WindowWidth");

	// Read window height from config file
	m_Window.Height = ConfigManager::GetInstance().GetInt("WindowHeight");

	// Initialize the window
	InitWindow();
}

D2D::Window::~Window()
{
	// Terminate glfw
	glfwTerminate();

}

void D2D::Window::InitWindow()
{
	// Initialize glfw
	glfwInit();

	// Set the error callback
	glfwSetErrorCallback(error_callback);

	// Read necesarry information from config file
	bool fullscreen{ConfigManager::GetInstance().GetBool("FullScreen")};
	auto title{ ConfigManager::GetInstance().GetString("WindowName") };
	int monitorIndex{ ConfigManager::GetInstance().GetInt("Monitor") };

	// If fullscreen is requested, create a fullscreen window, if not, create a normal window
	if (fullscreen)
	{
		CreateFullscreenWindow(title, monitorIndex);
	}
	else
	{
		CreateWindow(title, monitorIndex);
	}

	// If a window was successfully created, set the frame buffer resize callback function
	if (m_Window.pWindow != nullptr)
	{
		glfwSetFramebufferSizeCallback(m_Window.pWindow, resize_callback);
	}
}

void D2D::Window::CreateFullscreenWindow(const char* title, int monitorIndex)
{
	GLFWmonitor* monitor{};

	// Get the amount and a list of the monitors
	int count{};
	auto monitors{ glfwGetMonitors(&count) };

	// If the index is available, set the monitor, if not, set the primary monitor
	if (monitorIndex < count)
	{
		monitor = monitors[monitorIndex];
	}
	else
	{
		monitor = glfwGetPrimaryMonitor();
	}

	// Create the window
	m_Window.pWindow = glfwCreateWindow(m_Window.Width, m_Window.Height, title, monitor, nullptr);
}

void D2D::Window::CreateWindow(const char* title, int monitorIndex)
{
	GLFWmonitor* monitor{};

	// Get the amount and a list of all available monitors
	int count{};
	auto monitors{ glfwGetMonitors(&count) };

	// If the monitor index is available, set monitor
	if (monitorIndex < count)
	{
		monitor = monitors[monitorIndex];
	}

	// Create the window
	m_Window.pWindow = glfwCreateWindow(m_Window.Width, m_Window.Height, title, nullptr, nullptr);

	// If a monitor is requested, calculate and set a new position for the window on the requested monitor
	if (monitor != nullptr)
	{
		int x, y, w, h;

		// Get all the measurements of the monitor
		glfwGetMonitorWorkarea(monitor, &x, &y, &w, &h);

		// Calculate new position for the screen
		int newX{ x + (w / 2) - (m_Window.Width / 2) };
		int newY{ y + (h / 2) - (m_Window.Width / 2) };

		// Get refreshrate
		auto refreshRate{ glfwGetVideoMode(monitor)->refreshRate };

		// Set the new window position
		glfwSetWindowMonitor(m_Window.pWindow, nullptr, newX, newY, m_Window.Width, m_Window.Height, refreshRate);
	}
}

void D2D::Window::ResizeCallback(GLFWwindow* pWindow, int width, int height)
{
	// If the given window is the current window, set new values
	if (m_Window.pWindow == pWindow)
	{
		m_Window.Width = width;
		m_Window.Height = height;
		m_Window.FrameBufferResized = true;
	}
}

void D2D::Window::error_callback(int /*error*/, const char* description)
{
	// Print the error
	std::cout << "GLFW error: " << description << "\n";
}

void D2D::Window::resize_callback(GLFWwindow* pWindow, int width, int height)
{
	// Pass along values to non static callback function
	Window::GetInstance().ResizeCallback(pWindow, width, height);
}

