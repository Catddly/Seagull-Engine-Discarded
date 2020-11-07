#include "sgpch.h"
#include "DirectX12SwapChain.h"

#include "Core/Application.h"

namespace SG
{

	DirectX12SwapChain::DirectX12SwapChain(ComPtr<IDXGIFactory1> factory)
		:m_DxgiFactory(factory)
	{}

	void DirectX12SwapChain::Init(Window* window, const Ref<DirectX12Context>& context, const Ref<DirectX12RenderQueue>& renderCommand)
	{
		// Create swap chain
		m_SwapChain.Reset();

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		swapChainDesc.BufferDesc.Width = (UINT)window->GetWidth();
		swapChainDesc.BufferDesc.Height = (UINT)window->GetHeight();
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.Format = context->GetBackBufferFormat();
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = context->Get4xMSAAState() ? 4 : 1;
		swapChainDesc.SampleDesc.Quality = context->Get4xMSAAState() ? (context->Get4xMSAAQualityCount() - 1) : 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = swapChainBufferCount;
		swapChainDesc.OutputWindow = static_cast<HWND>(window->GetNativeWindow());
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		ThrowIfFailed(m_DxgiFactory->CreateSwapChain(renderCommand->GetCommandQueueNative(),
			&swapChainDesc,
			m_SwapChain.GetAddressOf()));
	}

	void DirectX12SwapChain::GetBuffer(UINT i, ComPtr<ID3D12Resource>m_SwapChainBuffer)
	{
		ThrowIfFailed(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(m_SwapChainBuffer.GetAddressOf())));
	}

	void DirectX12SwapChain::CreateRtv()
	{
		m_Rtv->CreateRtv();
	}

	void DirectX12SwapChain::CreateRtvDescHeap(ComPtr<ID3D12Device1> device)
	{
		m_Rtv = CreateRef<DirectX12Rtv>(device, this);
	}

	void DirectX12SwapChain::SwapBuffer()
	{
		// swap the back and front buffers
		ThrowIfFailed(m_SwapChain->Present(0, 0));
		m_Rtv->SwapCurrBuffer();
	}

	void DirectX12SwapChain::ResetBuffers()
	{
		for (int i = 0; i < swapChainBufferCount; ++i)
			m_Rtv->ResetBuffer(i);
	}

	void DirectX12SwapChain::ResizeBuffers()
	{
		// Resize the swap chain.
		ThrowIfFailed(m_SwapChain->ResizeBuffers(
			swapChainBufferCount,
			(UINT)Application::Get().GetWindow().GetWidth(),
			(UINT)Application::Get().GetWindow().GetHeight(),
			DirectX12Context::GetBackBufferFormat(),
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));
		m_Rtv->ResetCurrBackBufferIndex();
	}

}