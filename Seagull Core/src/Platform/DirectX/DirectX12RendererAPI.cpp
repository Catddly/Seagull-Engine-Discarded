#include "sgpch.h"
#include "DirectX12RendererAPI.h"

#include "DirectXHelper.h"
#include "Core/Application.h"
#include "d3dx12.h"

#include <DirectXColors.h>

namespace SG
{

	DirectX12RendererAPI::DirectX12RendererAPI()
	{
		// Enable debug layer
#if (defined(DEBUG) || defined(_DEBUG)) && defined(SG_DEBUG)
		{
			ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&m_DebugController)));
			m_DebugController->EnableDebugLayer();
		}
#endif

		// Create dxgiFactory
		ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&m_DxgiFactory)));

		// Try to create hardware device
		HRESULT hardwareResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_D3dDevice));
		if (FAILED(hardwareResult))
		{
			// use warp adapter (software device)
			ComPtr<IDXGIAdapter> warpAdapter;
			ThrowIfFailed(m_DxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));
			ThrowIfFailed(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_D3dDevice)));
		}
	}

	DirectX12RendererAPI::~DirectX12RendererAPI()
	{
#ifdef DX12_ENABLE_DEBUG_LAYER
		IDXGIDebug1* pDebug = NULL;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug))))
		{
			pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
			pDebug->Release();
		}
#endif
	}

	void DirectX12RendererAPI::Init()
	{
		DirectX12Context::Init(m_D3dDevice.Get());
		DirectX12Context::LogOutAdaptersToConsole(m_DxgiFactory.Get());

		m_RenderQueue = CreateRef<DirectX12RenderQueue>(m_D3dDevice.Get());
		// Command list should connect with a command allocator
		m_CommandList = CreateRef<DirectX12CommandList>(m_D3dDevice.Get(), m_RenderQueue);
		// the command list should be closed at first
		// when we first use command list, we should reset it. Before we reset it, we should close it
		m_CommandList->Close();
		m_SwapChain = CreateRef<DirectX12SwapChain>(m_DxgiFactory.Get(), m_RenderQueue);

		CreateRtvAndDsvDescriptorsHeap();
		OnWindowResize(0, 0, Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());
	}

	void DirectX12RendererAPI::CreateRtvAndDsvDescriptorsHeap()
	{
		m_RtvHeap = CreateRef<DirectX12DescriptorHeap>(m_D3dDevice.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2);
		m_DsvHeap = CreateRef<DirectX12DescriptorHeap>(m_D3dDevice.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);
		m_SrvHeap = CreateRef<DirectX12DescriptorHeap>(m_D3dDevice.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1,
			D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	}

	void DirectX12RendererAPI::CreateRtv()
	{
		for (int i = 0; i < 2; i++)
		{
			m_SwapChain->GetBuffer(i, m_SwapChainBuffers[i].GetPtr());
			m_RtvHeap->CreateRtv(m_D3dDevice.Get(), m_SwapChainBuffers[i].GetNativePtr(), i, nullptr);
		}
	}

	void DirectX12RendererAPI::CreateDsv()
	{
		// Create the depth/stencil buffer and view.
		D3D12_RESOURCE_DESC depthStencilDesc;
		depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthStencilDesc.Alignment = 0;
		depthStencilDesc.Width = Application::Get().GetWindow().GetWidth();
		depthStencilDesc.Height = Application::Get().GetWindow().GetHeight();
		depthStencilDesc.DepthOrArraySize = 1;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		depthStencilDesc.SampleDesc.Count = DirectX12Context::Get4xMSAAState() ? 4 : 1;
		depthStencilDesc.SampleDesc.Quality = DirectX12Context::Get4xMSAAState() ? (DirectX12Context::Get4xMSAAQualityCount() - 1) : 0;
		depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE optClear;
		optClear.Format = DirectX12Context::GetDepthStencilFormat();
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;
		ThrowIfFailed(m_D3dDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&depthStencilDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&optClear,
			IID_PPV_ARGS(&m_DepthStencilBuffer.GetPtr())));

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = DirectX12Context::GetDepthStencilFormat();
		dsvDesc.Texture2D.MipSlice = 0;
		m_DsvHeap->CreateDsv(m_D3dDevice.Get(), m_DepthStencilBuffer.GetNativePtr(), 0, &dsvDesc);
	}

	void DirectX12RendererAPI::Clear()
	{
		// Reuse the memory associated with command recording.
		// We can only reset when the associated command lists have finished execution on the GPU.
		m_RenderQueue->ResetAlloctor();

		// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
		// Reusing the command list reuses memory.
		m_CommandList->Reset(m_RenderQueue->GetCommandAllocatorNative(), nullptr);

		// Indicate a state transition on the resource usage.
		m_CommandList->ResourceBarrier(1, m_SwapChainBuffers[m_SwapChain->GetCurrBufferIndex()].GetNativePtr(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

		// Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
		m_CommandList->SetViewports(1, &m_ScreenViewport);
		m_CommandList->SetScissorRect(1, &m_ScissorRect);

		// Clear the back buffer and depth buffer.
		m_CommandList->ClearRtv(m_RtvHeap->GetView(m_SwapChain->GetCurrBufferIndex()), DirectX::Colors::Beige, 0, nullptr);
		m_CommandList->ClearDsv(m_DsvHeap->GetView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f,
			0, 0, nullptr);

		// Specify the buffers we are going to render to.
		m_CommandList->SetRenderTarget(1, &m_RtvHeap->GetView(m_SwapChain->GetCurrBufferIndex()), true, &m_DsvHeap->GetView());

		// Indicate a state transition on the resource usage.
		m_CommandList->ResourceBarrier(1, m_SwapChainBuffers[m_SwapChain->GetCurrBufferIndex()].GetNativePtr(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

		// Done recording commands.
		m_CommandList->Close();

		// Add the command list to the queue for execution.
		m_RenderQueue->ExecuteCommandLists(m_CommandList->GetCommandListNative());

		// swap the back and front buffers
		m_SwapChain->Present();

		// Wait until frame commands are complete.  This waiting is inefficient and is
		// done for simplicity.  Later we will show how to organize our rendering code
		// so we do not have to wait per frame.
		m_RenderQueue->FlushCommandQueue();
	}

	void DirectX12RendererAPI::OnWindowResize(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		assert(m_D3dDevice);
		assert(bool(m_SwapChain));
		assert(bool(m_RenderQueue));

		// Flush before changing any resources.
		m_RenderQueue->FlushCommandQueue();

		m_CommandList->Reset(m_RenderQueue->GetCommandAllocatorNative(), nullptr);

		// Release the previous resources we will be recreating.
		for (int i = 0; i < 2; ++i)
			m_SwapChainBuffers[i].Reset();
		m_DepthStencilBuffer.Reset();

		// Resize the swap chain.
		m_SwapChain->ResizeBuffer(2, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

		CreateRtv();
		CreateDsv();

		// Transition the resource from its initial state to be used as a depth buffer.
		m_CommandList->ResourceBarrier(1, m_DepthStencilBuffer.GetNativePtr(),
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);

		// Execute the resize commands.
		m_CommandList->Close();
		m_RenderQueue->ExecuteCommandLists(m_CommandList->GetCommandListNative());

		// Wait until resize is complete.
		m_RenderQueue->FlushCommandQueue();

		// Update the viewport transform to cover the client area.
		m_ScreenViewport.TopLeftX = 0;
		m_ScreenViewport.TopLeftY = 0;
		m_ScreenViewport.Width = static_cast<float>(width);
		m_ScreenViewport.Height = static_cast<float>(height);
		m_ScreenViewport.MinDepth = 0.0f;
		m_ScreenViewport.MaxDepth = 1.0f;
		
		m_ScissorRect = { static_cast<LONG>(x), static_cast<LONG>(y),
			static_cast<LONG>(width), static_cast<LONG>(height) };
	}

	void DirectX12RendererAPI::SetViewportSize(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		OnWindowResize(x, y, width, height);
	}

}