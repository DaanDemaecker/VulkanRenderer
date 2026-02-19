// IWindow.h
// This file will hold the parent and default classes of the window Interface

#ifndef _DDM_I_WINDOW_
#define _DDM_I_WINDOW_

namespace DDM
{
	struct WindowData
	{
		void* handle{};
		int width{};
		int height{};
		bool fullscreen{};
	};

	class IWindow
	{
	public:
		IWindow() = default;
		virtual ~IWindow() = default;

		IWindow(IWindow&) = delete;
		IWindow(IWindow&&) = delete;

		IWindow& operator=(IWindow&) = delete;
		IWindow& operator=(IWindow&&) = delete;

		virtual void CreateWindow() = 0;

		virtual void PollEvents() = 0;

		virtual bool ShouldClose() = 0;

		virtual const WindowData& GetWindowData() = 0;

		virtual void SetFullscreenMode(bool fullscreen) = 0;

		virtual void ToggleFullscreenMode() = 0;
	};

	class DefaultWindow final : public IWindow
	{
	public:
		DefaultWindow() = default;
		~DefaultWindow() = default;

		DefaultWindow(DefaultWindow&) = delete;
		DefaultWindow(DefaultWindow&&) = delete;

		DefaultWindow& operator=(DefaultWindow&) = delete;
		DefaultWindow& operator=(DefaultWindow&&) = delete;

		virtual void CreateWindow() override {}

		virtual void PollEvents() override {}

		virtual bool ShouldClose() override { return false; }

		virtual const WindowData& GetWindowData() override { return WindowData(); }

		virtual void SetFullscreenMode(bool fullscreen) override {}

		virtual void ToggleFullscreenMode() override {}
	};
}

#endif // !_DDM_I_WINDOW_
