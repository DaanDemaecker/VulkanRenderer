// Window.h
// This class will hold the singleton for the GLFW window

#ifndef WindowIncluded
#define WindowIncluded

// File includes
#include "Singleton.h"
#include "GLFWIncludes.h"

namespace D3D
{
	// Window struct
	// Member variables:
	//     -PWindow: pointer to the glfwWindow
	//     -width: windowWidth
	//     -height: windowHeight
	//     -frameBufferResized: will be set to true when window is resized
	struct WindowStruct
	{
		GLFWwindow* pWindow{};
		int Width;
		int Height;
		bool FrameBufferResized = false;
	};

	class Window : public Singleton<Window>
	{
	public:
		Window() = default;

		void CreateWindow(int width, int height);

		~Window();

		WindowStruct& GetWindowStruct() { return m_Window; }

		void SetFrameBufferResized(bool value);

	private:

		// Function that will initialize the glfw window
		void InitWindow();

		// Function needed for resizing of glfw window
		static void FramebufferResizeCallback(GLFWwindow* pWindow, int width, int height);

		WindowStruct m_Window;

	};
}

#endif // !WindowIncluded