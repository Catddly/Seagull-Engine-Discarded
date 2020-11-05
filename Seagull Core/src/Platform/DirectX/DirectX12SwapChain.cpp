#include "sgpch.h"
#include "DirectX12SwapChain.h"

namespace SG
{

	DirectX12SwapChain::DirectX12SwapChain(Window* window, ComPtr<IDXGIFactory4> factory, Ref<DirectX12RenderCommand> renderCommand)
		:m_Window(window), m_DxgiFactory(factory), m_RenderCommand(renderCommand)
	{
		// Create swap chain
		m_SwapChain.Reset();

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		swapChainDesc.BufferDesc.Width = (UINT)m_Window->GetWidth();
		swapChainDesc.BufferDesc.Height = (UINT)m_Window->GetHeight();
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.Format = DirectX12Context::GetBackBufferFormat();
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = DirectX12Context::Get4xMSAAState() ? 4 : 1;
		swapChainDesc.SampleDesc.Quality = DirectX12Context::Get4xMSAAState() ? (DirectX12Context::Get4xMSAAQualityCount() - 1) : 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = swapChainBufferCount;
		swapChainDesc.OutputWindow = static_cast<HWND>(m_Window->GetNativeWindow());
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		ComPtr<ID3D12CommandQueue> commandQueue = m_RenderCommand->GetCommandQueue();
		ThrowIfFailed(m_DxgiFactory->CreateSwapChain(commandQueue.Get(),
			&swapChainDesc,
			m_SwapChain.GetAddressOf()));
	}

	HRESULT DirectX12SwapChain::GetBuffer(int index, ComPtr<ID3D12Resource> resource)
	{
		return m_SwapChain->GetBuffer(index, IID_PPV_ARGS(&resource));
	}

}