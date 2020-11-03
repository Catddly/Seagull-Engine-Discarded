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

	private:
		static API s_API;
	};

}