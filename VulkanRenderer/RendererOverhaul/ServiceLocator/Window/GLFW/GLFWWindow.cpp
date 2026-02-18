// GLFWWindow.cpp

// Header include
#include "GLFWWindow.h"

// File includes
#include "Includes/GLFWIncludes.h"
#include "Engine/ConfigManager.h"
#include "ServiceLocator/ServiceLocator.h"

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
			WriteToFile();

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

		void PollEvents()
		{
			glfwPollEvents();
		}

		bool ShouldClose()
		{
			return glfwWindowShouldClose(static_cast<GLFWwindow*>(m_pWindow.handle));
		}

	private:
		// Window data
		WindowData m_pWindow{};

		// Path to init data
		std::string m_DataPath = "Data/Window.txt";

		// Single instance of GLFWImpl
		static GLFWImpl* m_sInstance;

		static void error_callback(int error, const char* description)
		{
			m_sInstance->HandleError(error, description);
		}

		void HandleError(int error, const char* description)
		{
			std::cout << "Glfw error: " << error << "\n" << description << "\n";
		}

		void WriteToFile()
		{
			auto& fileSystem = DDM::ServiceLocator::GetFileSystem();

			if (fileSystem.OpenWrite(m_DataPath))
			{
				auto initData = GetInitData();

				fileSystem.Write(m_DataPath, (const char*)&initData, sizeof(InitData));
			}

			fileSystem.CloseWrite(m_DataPath);
		}

		InitData GetInitData()
		{
			return InitData();
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
	if (m_pImpl != nullptr)
	{
		m_pImpl->CreateWindow();
	}
}

void DDM::GLFWWindow::PollEvents()
{
	if (m_pImpl != nullptr)
	{
		m_pImpl->PollEvents();
	}
}

bool DDM::GLFWWindow::ShouldClose()
{
	bool shouldClose = false;

	if (m_pImpl != nullptr)
	{
		shouldClose = m_pImpl->ShouldClose();
	}

	if (shouldClose)
	{
		m_pImpl.reset();
	}

	return shouldClose;
}
