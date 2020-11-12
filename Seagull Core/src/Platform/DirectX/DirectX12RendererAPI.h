#pragma once

#include "Renderer/RendererAPI.h"

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#include "Core/Core.h"
#include "DirectX12Context.h"
#include "DirectX12RenderQueue.h"
#include "DirectX12CommandList.h"
#include "DirectX12DescriptorHeap.h"
#include "DirectX12Resource.h"
#include "DirectX12SwapChain.h"

#if defined(_DEBUG) && defined(SG_DEBUG)
	#define DX12_ENABLE_DEBUG_LAYER
#endif

#ifdef DX12_ENABLE_DEBUG_LAYER
	#include <dxgidebug.h>
	#pragma comment(lib, "dxguid.lib")
#endif

// Link necessary d3d12 libraries.
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;

namespace SG
{

	class DirectX12RendererAPI final : public RendererAPI
	{
	public:
		DirectX12RendererAPI();
		DirectX12RendererAPI(const DirectX12RendererAPI&) = delete;
		DirectX12RendererAPI operator=(const DirectX12RendererAPI&) = delete;
		~DirectX12RendererAPI();

		virtual void Init() override;
		virtual void Clear() override;

		virtual void SetViewportSize(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
	private:
		void OnWindowResize(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		void CreateRtvAndDsvDescriptorsHeap();
		void CreateRtv();
		void CreateDsv();
	private:
#if (defined(DEBUG) | defined(_DEBUG)) & defined(SG_DEBUG)
		ComPtr<ID3D12Debug> m_DebugController;
#endif
		ComPtr<ID3D12Device1> m_D3dDevice;
		ComPtr<IDXGIFactory7> m_DxgiFactory;

		Ref<DirectX12SwapChain> m_SwapChain;

		Ref<DirectX12RenderQueue> m_RenderQueue;
		Ref<DirectX12CommandList> m_CommandList;

		Ref<DirectX12DescriptorHeap> m_RtvHeap;
		Ref<DirectX12DescriptorHeap> m_DsvHeap;
		DirectX12Resource m_SwapChainBuffers[2];
		DirectX12Resource m_DepthStencilBuffer;

		D3D12_VIEWPORT m_ScreenViewport = {};
		D3D12_RECT m_ScissorRect = {};
	};

}