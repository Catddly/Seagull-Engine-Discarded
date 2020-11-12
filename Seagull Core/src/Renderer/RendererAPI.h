#pragma once

namespace SG
{

	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, DirectX12 = 1
		};
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;
		virtual void Clear() = 0;

		virtual void SetViewportSize(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

		inline static API GetAPI() { return s_API; }
	private:
		static API s_API;
	};

}