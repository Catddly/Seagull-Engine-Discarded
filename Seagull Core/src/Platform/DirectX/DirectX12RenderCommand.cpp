#include "sgpch.h"
#include "DirectX12RenderCommand.h"

#include "DirectXHelper.h"
#include "Core/Application.h"

namespace SG
{

	DirectX12RenderCommand::DirectX12RenderCommand(ComPtr<ID3D12Device> device)
		: m_D3dDevice(device)
	{
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
	}

	void DirectX12RenderCommand::Submit()
	{

	}

	void DirectX12RenderCommand::SetViewport()
	{
		D3D12_VIEWPORT viewport;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = static_cast<float>(Application::Get().GetWindow().GetWidth());
		viewport.Height = static_cast<float>(Application::Get().GetWindow().GetHeight());
		viewport.MinDepth = 0.0f;  // normalized coordinate
		viewport.MaxDepth = 1.0f;

		m_CommandList->RSSetViewports(1, &viewport);
	}

	void DirectX12RenderCommand::SetScissorRectangle()
	{
		tagRECT scissorRect = { 0, 0, 
			Application::Get().GetWindow().GetWidth() / 2, 
			Application::Get().GetWindow().GetHeight() / 2 };

		m_CommandList->RSSetScissorRects(1, &scissorRect);
	}

}