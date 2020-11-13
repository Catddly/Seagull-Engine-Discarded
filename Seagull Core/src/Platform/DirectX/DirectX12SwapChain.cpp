#include "sgpch.h"
#include "DirectX12SwapChain.h"

#include "DirectXHelper.h"
#include "Core/Application.h"
#include "DirectX12Context.h"
#include "DirectXHelper.h"

namespace SG
{

	DirectX12SwapChain::DirectX12SwapChain(IDXGIFactory7* factory, const Ref<DirectX12RenderQueue>& renderQueue)
	{
		// Create swap chain
		m_SwapChain.Reset();

		m_SwapChainDesc = { };
		m_SwapChainDesc.BufferDesc.Width = (UINT)Application::Get().GetWindow()->GetWidth();
		m_SwapChainDesc.BufferDesc.Height = (UINT)Application::Get().GetWindow()->GetHeight();
		m_SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		m_SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		m_SwapChainDesc.BufferDesc.Format = DirectX12Context::GetBackBufferFormat();
		m_SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		m_SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		m_SwapChainDesc.SampleDesc.Count = DirectX12Context::Get4xMSAAState() ? 4 : 1;
		m_SwapChainDesc.SampleDesc.Quality = DirectX12Context::Get4xMSAAState() ? (DirectX12Context::Get4xMSAAQualityCount() - 1) : 0;
		m_SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		m_SwapChainDesc.BufferCount = 2;
		m_SwapChainDesc.OutputWindow = static_cast<HWND>(Application::Get().GetWindow()->GetNativeWindow());
		m_SwapChainDesc.Windowed = true;
		m_SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		m_SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		ThrowIfFailed(factory->CreateSwapChain(renderQueue->GetCommandQueueNative(),
			&m_SwapChainDesc, m_SwapChain.GetAddressOf()));
	}

	void DirectX12SwapChain::GetBuffer(UINT index, ComPtr<ID3D12Resource>& resource)
	{
		ThrowIfFailed(m_SwapChain->GetBuffer(index, IID_PPV_ARGS(&resource)));
	}

	void DirectX12SwapChain::ResizeBuffer(UINT bufferCount, UINT swapChainFormat)
	{
		ThrowIfFailed(m_SwapChain->ResizeBuffers(bufferCount,
			Application::Get().GetWindow()->GetWidth(),
			Application::Get().GetWindow()->GetHeight(),
			DirectX12Context::GetBackBufferFormat(),
			swapChainFormat));
		m_CurrBackBufferIndex = 0;
	}

	void DirectX12SwapChain::Present(UINT syncInterval, UINT flags)
	{
		ThrowIfFailed(m_SwapChain->Present(syncInterval, flags));
		m_CurrBackBufferIndex = (m_CurrBackBufferIndex + 1) % 2;
	}

}