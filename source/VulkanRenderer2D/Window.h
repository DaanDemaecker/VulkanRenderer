#ifndef WindowIncluded
#define WindowIncluded

// File includes
#include "Singleton.h"
#include "GLFWIncludes.h"

namespace D2D
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
		int Width{};
		int Height{};
		bool FrameBufferResized = false;
	};

	class Window : public Singleton<Window>
	{
	public:
		// Constructor
		Window();

		// Destructor
		~Window();

	private:
		// Window struct object
		WindowStruct m_Window{};

		// Initialize the window
		void InitWindow();

		// Create a fullscreen window
		// Parameters:
		//     title: a c-string of the window title
		//     monitorIndex: the index of the requested monitor
		void CreateFullscreenWindow(const char* title, int monitorIndex);

		// Create a non-fullscreen window
		// Parameters:
		//     title: a c-string of the window title
		//     monitorIndex: the index of the requested monitor
		void CreateWindow(const char* title, int monitorIndex);

		// A function that will be called when the window is resized
		// Parameters:
		//     pWindow: a pointer to the window in question
		//     width: the new width of the window
		//     height: the new height of the window
		void ResizeCallback(GLFWwindow* pWindow, int width, int height);

		// A static error callback function for glfw
		// Parameters:
		//     error: the index of the error type
		//     description: a c-string holding a description of the error
		static void error_callback(int error, const char* description);

		// A static resize callback funciton for glfw
		// Parameters:
		//     pWindow: a pointer to the window in question
		//     width: the new width of the window
		//     height: the new height of the window
		static void resize_callback(GLFWwindow* pWindow, int width, int height);

		// Function that is called when window is maximized or minimized
		// Parameters:
		//     pWindow: pointer to the window
		//     maximized: int that indicates if window was maximized or restored
		static void maximize_callback(GLFWwindow* pWindow, int maximized);
	};
}

#endif // !WindowIncluded