#pragma once

#include <wrl.h>
#include <d3d12.h>

using Microsoft::WRL::ComPtr;

namespace SG
{
	class DirectX12SwapChain;
	class DirectX12Context;

	class DirectX12Rtv
	{
	public:
		DirectX12Rtv(ComPtr<ID3D12Device1> device, DirectX12SwapChain* swapChain);
		~DirectX12Rtv() = default;

		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrBackBufferView();
		ComPtr<ID3D12Resource> GetCurrBackBuffer() { return m_SwapChainBuffers[m_CurrBackBuffer]; }
		void CreateRtv();

		void SwapCurrBuffer();
		void ResetBuffer(UINT index);
		void ResetCurrBackBufferIndex() { m_CurrBackBuffer = 0; }
	private:
		ComPtr<ID3D12Device1> m_D3dDevice;

		ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
		ComPtr<ID3D12Resource> m_SwapChainBuffers[2];
		DirectX12SwapChain* m_SwapChain;
		static int m_CurrBackBuffer;
	};

}