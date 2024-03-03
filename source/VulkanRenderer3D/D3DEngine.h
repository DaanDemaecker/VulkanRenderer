// D3DEngine.h
// This class will act as the main engine containing the game loop

#ifndef D3DEngineIncluded
#define D3DEngineIncluded

//File includes
#include "GLFWIncludes.h"

//Standard library includes
#include <functional>

namespace D3D
{
	class D3DEngine final
	{
	public:
		// Constructor
		// Parameters:
		//     - width: window width
		//     - height: window height
		D3DEngine(int width, int height);

		// Destructor
		~D3DEngine();

		// Function that will run the engine
		// Parameters:
		//     -load: function given that will be executed before game loop, used for loading in objects etc.
		void Run(const std::function<void()>& load);

		D3DEngine(const D3DEngine& other) = delete;
		D3DEngine(D3DEngine&& other) = delete;
		D3DEngine& operator=(const D3DEngine& other) = delete;
		D3DEngine& operator=(D3DEngine&& other) = delete;

	private:
		// Function that will initialize the glfw window
		void InitWindow();

		// Function needed for resizing of glfw window
		static void FramebufferResizeCallback(GLFWwindow* pWindow, int width, int height);
	};

	// Window struct
	// Member variables:
	//     -PWindow: pointer to the glfwWindow
	//     -width: windowWidth
	//     -height: windowHeight
	//     -frameBufferResized: will be set to true when window is resized
	struct Window
	{
		GLFWwindow* pWindow{};
		int Width;
		int Height;
		bool FrameBufferResized = false;
	};
}

#endif // !D3DEngineIncluded