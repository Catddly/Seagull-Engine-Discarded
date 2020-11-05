#pragma once

#include "Renderer/RendererAPI.h"

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#include "Core/Core.h"

// Link necessary d3d12 libraries.
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;

namespace SG
{
	static const UINT m_SwapChainBufferCount = 2;

	class DirectX12RendererAPI final : public RendererAPI
	{
	public:
		DirectX12RendererAPI();
		DirectX12RendererAPI(const DirectX12RendererAPI&) = delete;
		DirectX12RendererAPI operator=(const DirectX12RendererAPI&) = delete;
		~DirectX12RendererAPI();

		virtual void Init() override;
		virtual void Clear() override;

		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrBackBufferView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;
		ID3D12Resource* GetCurrBackBuffer() const;
	private:
		void LogAdapters();
		void LogAdapterOutputs(IDXGIAdapter* adapter);
		void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

		void FlushCommandQueue();
		void OnResize();
		void CreateRtvAndDsvDescriptorsHeap();
		//void CreateRtv();
		//void CreateDsv();
	private:
#if (defined(DEBUG) | defined(_DEBUG)) & defined(SG_DEBUG)
		ComPtr<ID3D12Debug> m_DebugController;
#endif
		ComPtr<ID3D12Device1> m_D3dDevice;
		ComPtr<IDXGIFactory7> m_DxgiFactory;
		ComPtr<IDXGISwapChain> m_SwapChain;

		ComPtr<ID3D12Fence1> m_Fence;
		UINT64 m_CurrFence = 0;

		ComPtr<ID3D12CommandQueue> m_CommandQueue;
		ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
		ComPtr<ID3D12GraphicsCommandList> m_CommandList;

		static int m_CurrBackBuffer;
		ComPtr<ID3D12Resource> m_SwapChainBuffer[m_SwapChainBufferCount];
		ComPtr<ID3D12Resource> m_DepthStencilBuffer;

		ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
		ComPtr<ID3D12DescriptorHeap> m_DsvHeap;

		D3D12_VIEWPORT m_ScreenViewport;
		D3D12_RECT m_ScissorRect;

		UINT m_RtvDescriptorSize = 0, m_DsvDescriptorSize = 0, m_CbvDescriptorSize = 0;  // each descriptor size of the device
		bool m_4xMSAAState = false;
		UINT m_4xMSAAQuality = 0;                                                // quality level of 4X MSAA
		DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT m_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	};

}