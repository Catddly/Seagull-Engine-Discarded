#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#include "Core/Window.h"
#include "DirectX12Context.h"
#include "DirectX12RenderCommand.h"

using Microsoft::WRL::ComPtr;

namespace SG
{
	//static const UINT swapChainBufferCount = 2;

	class DirectX12SwapChain
	{
	public:
		DirectX12SwapChain(Window* window, ComPtr<IDXGIFactory4> factory, Ref<DirectX12RenderCommand> renderCommand);
		~DirectX12SwapChain() = default;

		HRESULT GetBuffer(int index, ComPtr<ID3D12Resource> resource);
	private:
		ComPtr<IDXGISwapChain> m_SwapChain;
		Window* m_Window;
		ComPtr<IDXGIFactory4> m_DxgiFactory;
		Ref<DirectX12RenderCommand> m_RenderCommand;
	};

}