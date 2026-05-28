// Window.cpp

// Header include
#include "Window.h"

// File includes
#include "Includes/GLFWIncludes.h"
#include "ServiceLocator/ServiceLocator.h"
#include "Engine/ConfigManager.h"

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

	class GLFWImpl final
	{
	public:
		GLFWImpl()
		{
			m_sInstance = this;

			glfwInit();

			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

			glfwSetErrorCallback(error_callback);
		}

		~GLFWImpl()
		{
			WriteToFile();

			if (m_Window.handle != nullptr)
			{
				glfwDestroyWindow(GetGLFWHandle());
			}

			glfwTerminate();
		}

		void InitializeWindow()
		{
			if (m_Window.handle != nullptr)
			{
				return;
			}

			InitData initData = ReadInitData();

			//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

			auto handle = glfwCreateWindow(initData.width, initData.height, ConfigManager::GetInstance().GetString("WindowName"), nullptr, nullptr);


			glfwSetWindowSizeCallback(handle, resize_callback);
			glfwSetWindowPosCallback(handle, moved_callback);

			m_Window.handle = handle;
			m_Window.width = initData.width;
			m_Window.posX = initData.posX;
			m_Window.posY = initData.posY;
			m_Window.height = initData.height;
			m_Window.fullscreen = initData.fullscreen;

			SetWindowPosAndSize();
		}

		void PollEvents()
		{
			glfwPollEvents();
		}

		bool ShouldClose()
		{
			return glfwWindowShouldClose(GetGLFWHandle());
		}

		const WindowData& GetWindowData()
		{
			return m_Window;
		}

		void SetFullScreenMode(bool fullScreen)
		{
			m_Window.fullscreen = fullScreen;

			if (fullScreen)
			{
				GLFWmonitor* pMonitor = GetCurrentMonitor();

				glfwSetWindowMonitor(GetGLFWHandle(), pMonitor, 0, 0, m_Window.width, m_Window.height, GLFW_DONT_CARE);
			}
			else
			{
				glfwSetWindowMonitor(GetGLFWHandle(), nullptr, m_Window.posX, m_Window.posY, m_Window.width, m_Window.height, GLFW_DONT_CARE);
			}

		}

		void ToggleFullscreen()
		{
			SetFullScreenMode(!m_Window.fullscreen);
		}

		void SetDimensions(int x, int y)
		{
			m_Window.width = x;
			m_Window.height = y;

			SetWindowPosAndSize();
		}

		void GetFrameBufferSize(int& width, int& height)
		{
			glfwGetFramebufferSize(GetGLFWHandle(), &width, &height);
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

		// Default xpos
		const int m_DefaultYPos{ 50 };


		GLFWwindow* GetGLFWHandle() const
		{
			return static_cast<GLFWwindow*>(m_Window.handle);
		}

		static void error_callback(int error, const char* description)
		{
			std::cout << "Glfw error: " << error << "\n" << description << "\n";
		}

		static void resize_callback(GLFWwindow* /*window*/, int width, int height)
		{
			m_sInstance->Resized(width, height);
		}

		static void moved_callback(GLFWwindow* /*window*/, int x, int y)
		{
			m_sInstance->Moved(x, y);
		}

		void Resized(int width, int height)
		{
			m_Window.width = width;
			m_Window.height = height;
		}

		void Moved(int x, int y)
		{
			m_Window.posX = x;
			m_Window.posY = y;
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

			bool shouldReadFromData = ConfigManager::GetInstance().GetBool("ReadLastWindowState");

			auto& fileSystem = DDM::ServiceLocator::GetFileSystem();

			if (shouldReadFromData && fileSystem.OpenRead(m_DataPath))
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

			if (initData.posY <= 0)
			{
				initData.posY = m_DefaultYPos;
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

			auto handle = GetGLFWHandle();

			glfwGetWindowPos(handle, &initData.posX, &initData.posY);
			glfwGetWindowSize(handle, &initData.width, &initData.height);

			int maximized = glfwGetWindowAttrib(handle, GLFW_MAXIMIZED);

			initData.maximized = maximized == GLFW_TRUE;

			initData.fullscreen = m_Window.fullscreen;

			if (m_Window.fullscreen)
			{
				auto monitor = glfwGetWindowMonitor(handle);

				if (monitor == nullptr)
				{
					initData.monitor = 0;
				}
				else
				{
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
			}

			return initData;
		}

		void SetWindowPosAndSize()
		{
			GLFWmonitor* monitor = nullptr;

			if (m_Window.fullscreen)
			{
				monitor = GetCurrentMonitor();
			}

			glfwSetWindowMonitor(GetGLFWHandle(), monitor, m_Window.posX, m_Window.posY, m_Window.width, m_Window.height, GLFW_DONT_CARE);
		}

		GLFWmonitor* GetCurrentMonitor()
		{
			int lowesMonitorX{};
			int leftMonitor{};

			int monitorCount{};

			const auto monitors = glfwGetMonitors(&monitorCount);
			for (int i{}; i < monitorCount; ++i)
			{
				const auto monitor = monitors[i];

				int monitorX{};
				int monitorY{};

				glfwGetMonitorPos(monitor, &monitorX, &monitorY);

				if (monitorX < lowesMonitorX)
				{
					lowesMonitorX = monitorX;
					leftMonitor = i;
				}

				int monitorWidth{};
				int monitorHeight{};

				int modeCount{};

				const GLFWvidmode* modes = glfwGetVideoModes(monitor, &modeCount);

				for (int j{}; j < modeCount; ++j)
				{
					const auto currentMode = modes[j];

					if (currentMode.width > monitorWidth)
					{
						monitorWidth = currentMode.width;
					}
					
					if (currentMode.height > monitorHeight)
					{
						monitorHeight = currentMode.height;
					}
				}

				if (m_Window.posX >= monitorX &&
					m_Window.posX <= monitorX + monitorWidth &&
					m_Window.posY >= monitorY &&
					m_Window.posY <= monitorY + monitorHeight)
				{
					return monitor;
				}
			}

			if (m_Window.posX < lowesMonitorX)
			{
				return monitors[leftMonitor];
			}


			return glfwGetPrimaryMonitor();
		}
	};

	GLFWImpl* GLFWImpl::m_sInstance = nullptr;
}



DDM::Window::Window()
{
	m_pImpl = std::make_unique<GLFWImpl>();

	InitializeWindow();
}

DDM::Window::~Window()
{

}

void DDM::Window::PollEvents()
{
	m_pImpl->PollEvents();
}

bool DDM::Window::ShouldClose()
{
	return m_pImpl->ShouldClose();
}

const DDM::WindowData& DDM::Window::GetWindowData()
{
	return m_pImpl->GetWindowData();
}

void DDM::Window::SetFullscreenMode(bool fullscreen)
{
	m_pImpl->SetFullScreenMode(fullscreen);
}

void DDM::Window::ToggleFullscreenMode()
{
	m_pImpl->ToggleFullscreen();
}

void DDM::Window::SetDimensions(int x, int y)
{
	m_pImpl->SetDimensions(x, y);
}

void DDM::Window::GetFrameBufferSize(int& width, int& height)
{
	m_pImpl->GetFrameBufferSize(width, height);
}

void DDM::Window::InitializeWindow()
{
	m_pImpl->InitializeWindow();
}
