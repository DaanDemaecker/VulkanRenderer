// GLFWWindow.cpp

// Header include
#include "GLFWWindow.h"

// File includes
#include "Includes/GLFWIncludes.h"
#include "Engine/ConfigManager.h"

// Standard library includes
#include <iostream>

namespace DDM
{
	struct InitData
	{
		int width;
		int height;
		int Monitor;
		bool fullscreen;
		bool maximized;
	};

	class GLFWImpl
	{
	public:
		GLFWImpl()
		{
			m_sInstance = this;

			glfwInit();

			glfwSetErrorCallback(error_callback);
		}

		~GLFWImpl()
		{
			if (m_pWindow.handle != nullptr)
			{
				glfwDestroyWindow(static_cast<GLFWwindow*>(m_pWindow.handle));
			}

			glfwTerminate();
		}

		void CreateWindow()
		{
			m_pWindow.handle = glfwCreateWindow(800, 600, "test", nullptr, nullptr);
		}

	private:
		// Window data
		WindowData m_pWindow{};

		static GLFWImpl* m_sInstance;


		static void error_callback(int error, const char* description)
		{
			m_sInstance->HandleError(error, description);
		}

		void HandleError(int error, const char* description)
		{
			std::cout << "Glfw error: " << error << "\n" << description << "\n";
		}
	};


	GLFWImpl* GLFWImpl::m_sInstance = nullptr;
}


// GLFWWindow
DDM::GLFWWindow::GLFWWindow()
{
	m_pImpl = std::make_unique<GLFWImpl>();
}

DDM::GLFWWindow::~GLFWWindow()
{
}

void DDM::GLFWWindow::CreateWindow()
{
	m_pImpl->CreateWindow();
}
