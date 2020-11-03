#include "sgpch.h"
#include "RendererAPI.h"

namespace SG
{
#ifdef SG_GRAPHIC_API_DX12
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::DirectX12;
#else
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::None;
#endif
}