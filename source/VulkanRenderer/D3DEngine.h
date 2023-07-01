#pragma once
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
	};


	struct Window
	{
		GLFWwindow* pWindow{};
		int Width;
		int Height;
	};
}