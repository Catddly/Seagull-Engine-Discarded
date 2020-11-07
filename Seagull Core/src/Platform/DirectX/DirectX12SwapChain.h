#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#include "Core/Window.h"
#include "DirectX12Context.h"
#include "DirectX12RenderQueue.h"
#include "DirectX12Rtv.h"

using Microsoft::WRL::ComPtr;

namespace SG
{
	static const UINT swapChainBufferCount = 2;

	class DirectX12SwapChain
	{
	public:
		DirectX12SwapChain(ComPtr<IDXGIFactory1> factory);
		DirectX12SwapChain(const DirectX12SwapChain&) = delete;
		DirectX12SwapChain operator=(const DirectX12SwapChain& rhs) = delete;
		~DirectX12SwapChain() = default;

		void Init(Window* window, const Ref<DirectX12Context>& context, const Ref<DirectX12RenderQueue>& renderCommand);

		UINT GetSwapChainBufferCount() { return swapChainBufferCount; }

		void GetBuffer(UINT i, ComPtr<ID3D12Resource>m_SwapChainBuffer);
		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrBackBufferView() { return m_Rtv->GetCurrBackBufferView(); }
		ID3D12Resource* GetCurrBackBuffer() { return m_Rtv->GetCurrBackBuffer().Get(); }

		void CreateRtv();
		void CreateRtvDescHeap(ComPtr<ID3D12Device1> device);
		void SwapBuffer();
		void ResetBuffers();
		void ResizeBuffers();
	private:
		ComPtr<IDXGIFactory1> m_DxgiFactory;

		ComPtr<IDXGISwapChain> m_SwapChain;
		Ref<DirectX12Rtv> m_Rtv;
	};

}
