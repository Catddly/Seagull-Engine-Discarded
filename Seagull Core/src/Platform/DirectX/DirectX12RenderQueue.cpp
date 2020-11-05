#include "sgpch.h"
#include "DirectX12RenderQueue.h"

#include "DirectXHelper.h"

namespace SG
{

	DirectX12RenderQueue::DirectX12RenderQueue(ComPtr<ID3D12Device1> device, D3D12_COMMAND_QUEUE_DESC& cqDesc)
		:m_D3dDevice(device), commandQueueDesc(cqDesc)
	{}

	void DirectX12RenderQueue::Init()
	{
		// Create Fence
		ThrowIfFailed(m_D3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));

		ThrowIfFailed(m_D3dDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_CommandQueue)));
		ThrowIfFailed(m_D3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(m_CommandAllocator.GetAddressOf())));
	}

	void DirectX12RenderQueue::ResetAlloctor() const
	{
		ThrowIfFailed(m_CommandAllocator->Reset());
	}

	void DirectX12RenderQueue::ExecuteCommandLists(ID3D12GraphicsCommandList* graphicCommandList) const noexcept
	{
		ID3D12CommandList* cmdsLists[] = { graphicCommandList };
		m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	}

	void DirectX12RenderQueue::FlushCommandQueue()
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

}