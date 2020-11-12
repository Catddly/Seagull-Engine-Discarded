#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#include "DirectX12RenderQueue.h"

using Microsoft::WRL::ComPtr;

namespace SG
{

	class DirectX12SwapChain
	{
	public:
		DirectX12SwapChain(IDXGIFactory7* factory, const Ref<DirectX12RenderQueue>& renderQueue);
		DirectX12SwapChain(const DirectX12SwapChain&) = delete;
		DirectX12SwapChain operator=(const DirectX12SwapChain&) = delete;
		~DirectX12SwapChain() = default;

		void GetBuffer(UINT index, ComPtr<ID3D12Resource>& resource);
		void ResizeBuffer(UINT bufferCount, UINT swapChainFormat);

		void Present(UINT syncInterval = 0, UINT flags = 0);

		inline UINT GetCurrBufferIndex() const { return m_CurrBackBufferIndex; }

		operator bool() const { return m_SwapChain; }
	private:
		ComPtr<IDXGISwapChain> m_SwapChain;

		DXGI_SWAP_CHAIN_DESC m_SwapChainDesc;
		UINT m_CurrBackBufferIndex = 0;
	};

}