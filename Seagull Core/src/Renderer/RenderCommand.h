#pragma once

#include "Core/Core.h"
#include "RendererAPI.h"

#include <DirectXColors.h>

namespace SG
{

	class RenderCommand
	{
	public:
		static inline void Init()
		{
			s_RendererAPI->Init();
		}

		static inline void Clear()
		{
			s_RendererAPI->Clear();
		}
	private:
		static Scope<RendererAPI> s_RendererAPI;
	};

}