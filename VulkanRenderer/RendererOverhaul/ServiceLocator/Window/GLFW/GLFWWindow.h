// GLFWWindow.h
// This class will implement a window using the GLFW library

// Parent include
#include "ServiceLocator/Window/IWindow.h"

// File includes

// Standard library includes
#include <memory>

namespace DDM
{
	class GLFWImpl;

	class GLFWWindow final : public IWindow
	{
	public:
		GLFWWindow();
		~GLFWWindow();

		GLFWWindow(GLFWWindow&) = delete;
		GLFWWindow(GLFWWindow&&) = delete;

		GLFWWindow& operator=(GLFWWindow&) = delete;
		GLFWWindow& operator=(GLFWWindow&&) = delete;

		virtual void CreateWindow() override;

		virtual void PollEvents() override;

		virtual bool ShouldClose() override;
	private:
		std::unique_ptr<GLFWImpl> m_pImpl{};
	};
}