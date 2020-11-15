#include "sgpch.h"
#include "RenderCommand.h"

#include "Platform/DirectX/DirectX12RendererAPI.h"

namespace SG
{
	RendererAPI* RenderCommand::s_RendererAPI = new DirectX12RendererAPI();
}