#include "sgpch.h"
#include "DirectX12RendererAPI.h"

#include "DirectXHelper.h"
#include "Core/Application.h"
#include "d3dx12.h"

#include <DirectXColors.h>

namespace SG
{
	int DirectX12RendererAPI::m_CurrBackBuffer = 0;

	DirectX12RendererAPI::DirectX12RendererAPI()
	{
	}

	void DirectX12RendererAPI::Init()
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
			// use warp adapter
			ComPtr<IDXGIAdapter> warpAdapter;
			ThrowIfFailed(m_DxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));
			ThrowIfFailed(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_D3dDevice)));
		}

		//m_Context = CreateRef<DirectX12Context>(m_D3dDevice);
		// Create Fence
		ThrowIfFailed(m_D3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));

		// Get the descriptor size of the device
		m_RtvDescriptorSize = m_D3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		m_DsvDescriptorSize = m_D3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		m_CbvDescriptorSize = m_D3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		// Check the support of 4X MSAA
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS sampleQualityLevels;
		sampleQualityLevels.Format = m_BackBufferFormat;
		sampleQualityLevels.SampleCount = 4;
		sampleQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		sampleQualityLevels.NumQualityLevels = 0;
		ThrowIfFailed(m_D3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
			&sampleQualityLevels,
			sizeof(sampleQualityLevels)));

		m_4xMSAAQuality = sampleQualityLevels.NumQualityLevels;
		SG_CORE_ASSERT(m_4xMSAAQuality > 0, "Unexcepted MSAA quality level!");

		//m_RenderCommand = CreateRef<DirectX12RenderCommand>(m_D3dDevice);
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

		ThrowIfFailed(m_D3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue)));
		ThrowIfFailed(m_D3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(m_CommandAllocator.GetAddressOf())));
		// Command list should connect with a command allocator
		ThrowIfFailed(m_D3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get(),
			nullptr /* PSO */, IID_PPV_ARGS(m_CommandList.GetAddressOf())));

		// the command list should be closed at first
		// when we first use command list, we should reset it. Before we reset it, we should close it
		m_CommandList->Close();

		//m_SwapChain = CreateRef<DirectX12SwapChain>(&Application::Get().GetWindow(), m_DxgiFactory, m_RenderCommand);
		// Create swap chain
		m_SwapChain.Reset();

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		swapChainDesc.BufferDesc.Width = (UINT)Application::Get().GetWindow().GetWidth();
		swapChainDesc.BufferDesc.Height = (UINT)Application::Get().GetWindow().GetHeight();
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.Format = m_BackBufferFormat;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = m_4xMSAAState ? 4 : 1;
		swapChainDesc.SampleDesc.Quality = m_4xMSAAState ? (m_4xMSAAQuality - 1) : 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = m_SwapChainBufferCount;
		swapChainDesc.OutputWindow = static_cast<HWND>(Application::Get().GetWindow().GetNativeWindow());
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		ThrowIfFailed(m_DxgiFactory->CreateSwapChain(m_CommandQueue.Get(),
			&swapChainDesc,
			m_SwapChain.GetAddressOf()));

		CreateRtvAndDsvDescriptorsHeap();

		OnResize();

		LogAdapters();
	}

	void DirectX12RendererAPI::CreateRtvAndDsvDescriptorsHeap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvDesc;
		rtvDesc.NumDescriptors = m_SwapChainBufferCount;
		rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvDesc.NodeMask = 0;
		ThrowIfFailed(m_D3dDevice->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(m_RtvHeap.GetAddressOf())));

		D3D12_DESCRIPTOR_HEAP_DESC dsvDesc;
		dsvDesc.NumDescriptors = 1;
		dsvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvDesc.NodeMask = 0;
		ThrowIfFailed(m_D3dDevice->CreateDescriptorHeap(&dsvDesc, IID_PPV_ARGS(m_DsvHeap.GetAddressOf())));
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12RendererAPI::GetCurrBackBufferView() const
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_RtvHeap->GetCPUDescriptorHandleForHeapStart(),
			m_CurrBackBuffer,
			m_RtvDescriptorSize);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12RendererAPI::GetDepthStencilView() const
	{
		return m_DsvHeap->GetCPUDescriptorHandleForHeapStart();
	}

	//void DirectX12RendererAPI::CreateRtv()
	//{
	//	// for each buffer, create a rtv
	//	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
	//	for (int i = 0; i < swapChainBufferCount; i++)
	//	{
	//		ThrowIfFailed(m_SwapChain->GetBuffer(i, m_SwapChainBuffer[i]));
	//		m_D3dDevice->CreateRenderTargetView(m_SwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
	//		// Offset to the next buffer
	//		rtvHeapHandle.Offset(1, DirectX12Context::GetRtvDescriptorSize());
	//	}
	//}

	//void DirectX12RendererAPI::CreateDsv()
	//{
	//	// create dsv
	//	D3D12_RESOURCE_DESC dsvDesc;
	//	dsvDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	//	dsvDesc.Alignment = 0;
	//	dsvDesc.Width = Application::Get().GetWindow().GetWidth();
	//	dsvDesc.Height = Application::Get().GetWindow().GetHeight();
	//	dsvDesc.DepthOrArraySize = 1;
	//	dsvDesc.MipLevels = 1;
	//	dsvDesc.Format = DirectX12Context::GetDepthStencilFormat();
	//	dsvDesc.SampleDesc.Count = DirectX12Context::Get4xMSAAState() ? 4 : 1;
	//	dsvDesc.SampleDesc.Quality = DirectX12Context::Get4xMSAAState() ? (DirectX12Context::Get4xMSAAQualityCount() - 1) : 0;
	//	dsvDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	//	dsvDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	DirectX12RendererAPI::~DirectX12RendererAPI()
	{
	}

	void DirectX12RendererAPI::Clear()
	{
		// Reuse the memory associated with command recording.
// We can only reset when the associated command lists have finished execution on the GPU.
		ThrowIfFailed(m_CommandAllocator->Reset());

		// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
		// Reusing the command list reuses memory.
		ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr));

		// Indicate a state transition on the resource usage.
		m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(GetCurrBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		// Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
		m_CommandList->RSSetViewports(1, &m_ScreenViewport);
		m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

		// Clear the back buffer and depth buffer.
		m_CommandList->ClearRenderTargetView(GetCurrBackBufferView(), DirectX::Colors::Beige, 0, nullptr);
		m_CommandList->ClearDepthStencilView(GetDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		// Specify the buffers we are going to render to.
		m_CommandList->OMSetRenderTargets(1, &GetCurrBackBufferView(), true, &GetDepthStencilView());

		// Indicate a state transition on the resource usage.
		m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(GetCurrBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		// Done recording commands.
		ThrowIfFailed(m_CommandList->Close());

		// Add the command list to the queue for execution.
		ID3D12CommandList* cmdsLists[] = { m_CommandList.Get() };
		m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// swap the back and front buffers
		ThrowIfFailed(m_SwapChain->Present(0, 0));
		m_CurrBackBuffer = (m_CurrBackBuffer + 1) % m_SwapChainBufferCount;

		// Wait until frame commands are complete.  This waiting is inefficient and is
		// done for simplicity.  Later we will show how to organize our rendering code
		// so we do not have to wait per frame.
		FlushCommandQueue();
	}

	ID3D12Resource* DirectX12RendererAPI::GetCurrBackBuffer() const
	{
		return m_SwapChainBuffer[m_CurrBackBuffer].Get();
	}

	void DirectX12RendererAPI::FlushCommandQueue()
	{
		// Advance the fence value to mark commands up to this fence point.
		m_CurrFence++;

		// Add an instruction to the command queue to set a new fence point.  Because we 
		// are on the GPU timeline, the new fence point won't be set until the GPU finishes
		// processing all the commands prior to this Signal().
		ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), m_CurrFence));

		// Wait until the GPU has completed commands up to this fence point.
		if (m_Fence->GetCompletedValue() < m_CurrFence)
		{
			HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

			// Fire event when GPU hits current fence.  
			ThrowIfFailed(m_Fence->SetEventOnCompletion(m_CurrFence, eventHandle));

			// Wait until the GPU hits current fence event is fired.
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}

	void DirectX12RendererAPI::OnResize()
	{
		assert(m_D3dDevice);
		assert(m_SwapChain);
		assert(m_CommandAllocator);

		uint32_t width = Application::Get().GetWindow().GetWidth();
		uint32_t height = Application::Get().GetWindow().GetHeight();

		// Flush before changing any resources.
		FlushCommandQueue();

		ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr));

		// Release the previous resources we will be recreating.
		for (int i = 0; i < m_SwapChainBufferCount; ++i)
			m_SwapChainBuffer[i].Reset();
		m_DepthStencilBuffer.Reset();

		// Resize the swap chain.
		ThrowIfFailed(m_SwapChain->ResizeBuffers(
			m_SwapChainBufferCount,
			width,
			height,
			m_BackBufferFormat,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

		m_CurrBackBuffer = 0;

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (UINT i = 0; i < m_SwapChainBufferCount; i++)
		{
			ThrowIfFailed(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_SwapChainBuffer[i])));
			m_D3dDevice->CreateRenderTargetView(m_SwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
			rtvHeapHandle.Offset(1, m_RtvDescriptorSize);
		}

		// Create the depth/stencil buffer and view.
		D3D12_RESOURCE_DESC depthStencilDesc;
		depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthStencilDesc.Alignment = 0;
		depthStencilDesc.Width = width;
		depthStencilDesc.Height = height;
		depthStencilDesc.DepthOrArraySize = 1;
		depthStencilDesc.MipLevels = 1;

		// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
		// the depth buffer.  Therefore, because we need to create two views to the same resource:
		//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
		//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
		// we need to create the depth buffer resource with a typeless format.  
		depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

		depthStencilDesc.SampleDesc.Count = m_4xMSAAState ? 4 : 1;
		depthStencilDesc.SampleDesc.Quality = m_4xMSAAState ? (m_4xMSAAQuality - 1) : 0;
		depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE optClear;
		optClear.Format = m_DepthStencilFormat;
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;
		ThrowIfFailed(m_D3dDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&depthStencilDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&optClear,
			IID_PPV_ARGS(m_DepthStencilBuffer.GetAddressOf())));

		// Create descriptor to mip level 0 of entire resource using the format of the resource.
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = m_DepthStencilFormat;
		dsvDesc.Texture2D.MipSlice = 0;
		m_D3dDevice->CreateDepthStencilView(m_DepthStencilBuffer.Get(), &dsvDesc, GetDepthStencilView());

		// Transition the resource from its initial state to be used as a depth buffer.
		m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_DepthStencilBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

		// Execute the resize commands.
		ThrowIfFailed(m_CommandList->Close());
		ID3D12CommandList* cmdsLists[] = { m_CommandList.Get() };
		m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// Wait until resize is complete.
		FlushCommandQueue();

		// Update the viewport transform to cover the client area.
		m_ScreenViewport.TopLeftX = 0;
		m_ScreenViewport.TopLeftY = 0;
		m_ScreenViewport.Width = static_cast<float>(width);
		m_ScreenViewport.Height = static_cast<float>(height);
		m_ScreenViewport.MinDepth = 0.0f;
		m_ScreenViewport.MaxDepth = 1.0f;

		m_ScissorRect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
	}

	void DirectX12RendererAPI::LogAdapters()
	{
		UINT i = 0;
		IDXGIAdapter* adapter = nullptr;
		std::vector<IDXGIAdapter*> adapterList;
		while (m_DxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_ADAPTER_DESC desc;
			adapter->GetDesc(&desc);

			std::wstring text = L"***Adapter: ";
			text += desc.Description;

			SG_CORE_INFO(text);

			adapterList.push_back(adapter);
			++i;
		}

		for (size_t i = 0; i < adapterList.size(); ++i)
		{
			LogAdapterOutputs(adapterList[i]);
			ReleaseCom(adapterList[i]);
		}
	}

	void DirectX12RendererAPI::LogAdapterOutputs(IDXGIAdapter* adapter)
	{
		UINT i = 0;
		IDXGIOutput* output = nullptr;
		while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_OUTPUT_DESC desc;
			output->GetDesc(&desc);

			std::wstring text = L"***Output: ";
			text += desc.DeviceName;
			SG_CORE_INFO(text);

			LogOutputDisplayModes(output, m_BackBufferFormat);

			ReleaseCom(output);
			++i;
		}
	}

	void DirectX12RendererAPI::LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format)
	{
		UINT count = 0;
		UINT flags = 0;

		// Call with nullptr to get list count.
		output->GetDisplayModeList(format, flags, &count, nullptr);

		std::vector<DXGI_MODE_DESC> modeList(count);
		output->GetDisplayModeList(format, flags, &count, &modeList[0]);

		for (auto& x : modeList)
		{
			UINT n = x.RefreshRate.Numerator;
			UINT d = x.RefreshRate.Denominator;
			std::wstring text =
				L"  Width = " + std::to_wstring(x.Width) +
				L"  Height = " + std::to_wstring(x.Height) +
				L"  Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d);

			SG_CORE_INFO(text);
		}
	}

}