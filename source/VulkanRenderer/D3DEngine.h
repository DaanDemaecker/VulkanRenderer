#ifndef D3DEngineIncluded
#define D3DEngineIncluded


#include <string>
#include <functional>

namespace D3D
{
	class D3DEngine
	{
	public:
		D3DEngine(int width, int height);
		~D3DEngine();
		void Run(const std::function<void()>& load);

		D3DEngine(const D3DEngine& other) = delete;
		D3DEngine(D3DEngine&& other) = delete;
		D3DEngine& operator=(const D3DEngine& other) = delete;
		D3DEngine& operator=(D3DEngine&& other) = delete;

	private:
		void InitWindow();
		static void FramebufferResizeCallback(GLFWwindow* pWindow, int width, int height);
	};


	struct Window
	{
		GLFWwindow* pWindow{};
		int Width;
		int Height;
		bool FrameBufferResized = false;
	};
}

#endif // !D3DEngineIncluded