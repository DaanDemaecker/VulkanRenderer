// GLFWWindow.cpp

// Header include
#include "GLFWWindow.h"

// File includes
#include "Includes/GLFWIncludes.h"
#include "Engine/ConfigManager.h"
#include "ServiceLocator/ServiceLocator.h"
#include "Includes/GLMIncludes.h"

// Standard library includes
#include <iostream>

namespace DDM
{
	struct InitData
	{
		int posX;
		int posY;
		int width;
		int height;
		int monitor;
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

			if (m_Window.handle != nullptr)
			{
				glfwDestroyWindow(static_cast<GLFWwindow*>(m_Window.handle));
			}

			glfwTerminate();
		}

		void CreateWindow()
		{
			InitData initData = ReadInitData();

			auto handle = glfwCreateWindow(initData.width, initData.height, "test", nullptr, nullptr);


			glfwSetWindowSizeCallback(handle, resize_callback);

			m_Window.handle = handle;
			m_Window.width = initData.width;
			m_Window.height = initData.height;

			GLFWmonitor* monitor = nullptr;

			if (initData.fullscreen)
			{
				int count = 0;

				auto monitors = glfwGetMonitors(&count);

				if (count > initData.monitor)
				{
					monitor = monitors[initData.monitor];
				}
				else
				{
					monitor = glfwGetPrimaryMonitor();
				}
			}

			glfwSetWindowMonitor(handle, monitor, initData.posX, initData.posY, initData.width, initData.height, GLFW_DONT_CARE);
		}

		void PollEvents()
		{
			glfwPollEvents();
		}

		bool ShouldClose()
		{
			return glfwWindowShouldClose(static_cast<GLFWwindow*>(m_Window.handle));
		}

		const WindowData& GetWindowData()
		{
			return m_Window;
		}

		void SetFullScreenMode(bool fullScreen)
		{
			m_Window.fullscreen = fullScreen;

			GLFWwindow* handle = static_cast<GLFWwindow*>(m_Window.handle);

			if (fullScreen)
			{
				glfwSetWindowMonitor(handle, glfwGetPrimaryMonitor(), 0, 0, m_Window.width, m_Window.height, GLFW_DONT_CARE);
			}
			else
			{
				glfwSetWindowMonitor(handle, nullptr, 0, 0, m_Window.width, m_Window.height, GLFW_DONT_CARE);
			}

		}

		void ToggleFullscreen()
		{
			SetFullScreenMode(!m_Window.fullscreen);

			std::cout << "fullscreen toggled \n";
		}

	private:
		// Window data
		WindowData m_Window{};

		// Path to init data
		std::string m_DataPath = "Data/Window.txt";

		// Single instance of GLFWImpl
		static GLFWImpl* m_sInstance;

		// Default width
		const int m_DefaultWidth{ 800 };

		// Default height
		const int m_DefaultHeight{ 600 };

		static void error_callback(int error, const char* description)
		{
			std::cout << "Glfw error: " << error << "\n" << description << "\n";
		}

		static void resize_callback(GLFWwindow* window, int width, int height)
		{
			m_sInstance->Resized(width, height);
		}

		void Resized(int width, int height)
		{
			m_Window.width = width;
			m_Window.height = height;
		}

		void WriteToFile()
		{
			auto& fileSystem = DDM::ServiceLocator::GetFileSystem();

			if (fileSystem.OpenWrite(m_DataPath))
			{
				auto initData = GetFinalInitData();

				fileSystem.Write(m_DataPath, (const char*)&initData, sizeof(InitData));
			}

			fileSystem.CloseWrite(m_DataPath);
		}

		InitData ReadInitData()
		{
			InitData initData{};

			auto& fileSystem = DDM::ServiceLocator::GetFileSystem();

			if (fileSystem.OpenRead(m_DataPath))
			{
				fileSystem.Read(m_DataPath, (char*)&initData, sizeof(InitData));

				fileSystem.CloseRead(m_DataPath);

				if (initData.width == 0 || initData.height == 0)
				{
					ReadInitFromConfig(initData);
				}

			}
			else
			{
				ReadInitFromConfig(initData);
			}

			if (initData.width <= 0)
			{
				initData.width = m_DefaultWidth;
			}

			if (initData.height <= 0)
			{
				initData.height = m_DefaultHeight;
			}

			return initData;
		}

		void ReadInitFromConfig(InitData& initData)
		{
			auto& config = ConfigManager::GetInstance();

			initData.width = config.GetInt("WindowWidth");
			initData.height = config.GetInt("WindowHeight");
			initData.monitor = config.GetInt("Monitor");
			initData.fullscreen = config.GetBool("FullScreen");
			initData.maximized = config.GetBool("Maximized");
		}

		InitData GetFinalInitData()
		{
			auto initData = InitData();

			auto handle = static_cast<GLFWwindow*>(m_Window.handle);

			glfwGetWindowPos(handle, &initData.posX, &initData.posY);
			glfwGetWindowSize(handle, &initData.width, &initData.height);

			int maximized = glfwGetWindowAttrib(handle, GLFW_MAXIMIZED);

			initData.maximized = maximized == GLFW_TRUE;


			auto monitor = glfwGetWindowMonitor(handle);

			if (monitor == nullptr)
			{
				initData.fullscreen = true;
				initData.monitor = 1;
			}
			else
			{
				initData.fullscreen = true;

				int monitorCount;

				auto monitors = glfwGetMonitors(&monitorCount);

				for (int i{}; i < monitorCount; ++i)
				{
					if (monitor == monitors[i])
					{
						initData.monitor = i;
						break;
					}
				}
			}

			return initData;
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

const DDM::WindowData& DDM::GLFWWindow::GetWindowData()
{
	return m_pImpl->GetWindowData();
}

void DDM::GLFWWindow::SetFullscreenMode(bool fullscreen)
{
	m_pImpl->SetFullScreenMode(fullscreen);
}

void DDM::GLFWWindow::ToggleFullscreenMode()
{
	m_pImpl->ToggleFullscreen();
}
