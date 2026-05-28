// Window.h
// This class will hold a singleton representing the window

#ifndef _DDM_WINDOW_
#define _DDM_WINDOW_

// Parent include
#include "Engine/Singleton.h"

// Standard library includes
#include <memory>

namespace DDM
{
	class GLFWImpl;

	struct WindowData
	{
		void* handle{};
		int width{};
		int height{};
		int posX{};
		int posY{};
		bool fullscreen{};
	};

	class Window final : public DDM::Singleton<Window>
	{
	public:
		~Window();

		void PollEvents();

		bool ShouldClose();

		const WindowData& GetWindowData();

		void SetFullscreenMode(bool fullscreen);

		void ToggleFullscreenMode();

		void SetDimensions(int x, int y);

		void GetFrameBufferSize(int& width, int& height);
	private:
		friend class Singleton<Window>;
		Window();

		std::unique_ptr<GLFWImpl> m_pImpl;

		/// <summary>
		/// Create the window
		/// </summary>
		void InitializeWindow();
	};
}

#endif // !_DDM_WINDOW_