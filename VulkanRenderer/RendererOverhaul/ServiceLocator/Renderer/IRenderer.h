// IRenderer.h
// This class will hold the base class and default class for the renderer interface

#ifndef _DDM_I_RENDERER_
#define _DDM_I_RENDERER_

namespace DDM::Rendering
{
	class IRenderer
	{
	public:
		IRenderer() = default;
		virtual ~IRenderer() = default;

		IRenderer(IRenderer&) = delete;
		IRenderer(IRenderer&&) = delete;

		IRenderer& operator=(IRenderer&) = delete;
		IRenderer& operator=(IRenderer&&) = delete;

		virtual void Init() = 0;

		virtual void Render() = 0;

		virtual void Terminate() = 0;
	};


	class DefaultRenderer final : public IRenderer
	{
	public:
		DefaultRenderer() = default;
		virtual ~DefaultRenderer() = default;

		DefaultRenderer(DefaultRenderer&) = delete;
		DefaultRenderer(DefaultRenderer&&) = delete;

		DefaultRenderer& operator=(DefaultRenderer&) = delete;
		DefaultRenderer& operator=(DefaultRenderer&&) = delete;

		virtual void Render() override {}

		virtual void Init() override {}

		virtual void Terminate() override {}
	};
}

#endif // !DDM_I_RENDERER
