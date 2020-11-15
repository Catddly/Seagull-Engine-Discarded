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

		static inline void SetViewportSize(uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetViewportSize(0, 0, width, height);
		}
	private:
		friend class ImGuiLayer;

		static inline RendererAPI* GetRenderAPIInstance() { return s_RendererAPI; };
		static RendererAPI* s_RendererAPI;
	};

}