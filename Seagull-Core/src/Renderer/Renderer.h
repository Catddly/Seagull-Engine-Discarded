#pragma once

#include "Renderer/RendererAPI.h"

namespace SG
{

	class Renderer
	{
	public:
		Renderer() = default;
		virtual ~Renderer() = default;

		static void Init();
		static void Shutdown();

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:

	};

}