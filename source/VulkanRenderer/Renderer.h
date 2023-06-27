#pragma once

namespace D3D
{
	class Renderer
	{
	public:
		Renderer() = default;
		virtual ~Renderer() = default;

		Renderer(Renderer& other) = delete;
		Renderer(Renderer&& other) = delete;

		Renderer& operator=(Renderer& other) = delete;
		Renderer& operator=(Renderer&& other) = delete;

	};

	class DefaultRenderer final : public Renderer
	{
	public:
		DefaultRenderer() = default;
		virtual ~DefaultRenderer() = default;
	};
}