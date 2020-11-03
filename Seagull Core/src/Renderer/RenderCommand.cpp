#include "sgpch.h"
#include "RenderCommand.h"

#include "Platform/DirectX/DirectX12RendererAPI.h"

namespace SG
{
	Scope<RendererAPI> RenderCommand::s_RendererAPI = CreateScope<DirectX12RendererAPI>();
}