#include "sgpch.h"
#include "DirectX12Rtv.h"

#include "d3dx12.h"
#include "DirectX12Context.h"
#include "DirectX12SwapChain.h"

namespace SG
{

	int DirectX12Rtv::m_CurrBackBuffer = 0;

	DirectX12Rtv::DirectX12Rtv(ComPtr<ID3D12Device1> device, DirectX12SwapChain* swapChain)
		:m_D3dDevice(device), m_SwapChain(swapChain)
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvDesc;
		rtvDesc.NumDescriptors = m_SwapChain->GetSwapChainBufferCount();
		rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvDesc.NodeMask = 0;
		ThrowIfFailed(m_D3dDevice->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(m_RtvHeap.GetAddressOf())));
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12Rtv::GetCurrBackBufferView()
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_RtvHeap->GetCPUDescriptorHandleForHeapStart(),
			m_CurrBackBuffer,
			DirectX12Context::GetRtvDescriptorSize());
	}

	void DirectX12Rtv::CreateRtv()
	{
		// for each buffer, create a rtv
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (int i = 0; i < m_SwapChain->GetSwapChainBufferCount(); i++)
		{
			m_SwapChain->GetBuffer(i, m_SwapChainBuffers[i]);
			m_D3dDevice->CreateRenderTargetView(m_SwapChainBuffers[i].Get(), nullptr, rtvHeapHandle);
			// Offset to the next buffer
			rtvHeapHandle.Offset(1, DirectX12Context::GetRtvDescriptorSize());
		}
	}

	void DirectX12Rtv::SwapCurrBuffer()
	{
		m_CurrBackBuffer = (m_CurrBackBuffer + 1) % m_SwapChain->GetSwapChainBufferCount();
	}

	void DirectX12Rtv::ResetBuffer(UINT index)
	{
		m_SwapChainBuffers[index].Reset();
	}

}