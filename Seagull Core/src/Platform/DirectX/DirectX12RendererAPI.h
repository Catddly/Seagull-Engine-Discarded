#pragma once

#include "Renderer/RendererAPI.h"

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#include "Core/Core.h"
#include "DirectX12Context.h"
#include "DirectX12RenderQueue.h"
#include "DirectX12CommandList.h"

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

		void OnResize();
		void CreateRtvAndDsvDescriptorsHeap();
		void CreateRtv();
		//void CreateDsv();
	private:
#if (defined(DEBUG) | defined(_DEBUG)) & defined(SG_DEBUG)
		ComPtr<ID3D12Debug> m_DebugController;
#endif
		ComPtr<ID3D12Device1> m_D3dDevice;
		ComPtr<IDXGIFactory7> m_DxgiFactory;
		ComPtr<IDXGISwapChain> m_SwapChain;

		static int m_CurrBackBuffer;
		ComPtr<ID3D12Resource> m_SwapChainBuffer[m_SwapChainBufferCount];
		ComPtr<ID3D12Resource> m_DepthStencilBuffer;

		ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
		ComPtr<ID3D12DescriptorHeap> m_DsvHeap;

		D3D12_VIEWPORT m_ScreenViewport;
		D3D12_RECT m_ScissorRect;

		Ref<DirectX12Context> m_Context;
		Ref<DirectX12RenderQueue> m_RenderQueue;
		Ref<DirectX12CommandList> m_CommandList;
	};

}