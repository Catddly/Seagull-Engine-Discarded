#pragma once

#include <wrl.h>
#include <d3d12.h>

using Microsoft::WRL::ComPtr;

namespace SG
{

	class DirectX12RenderQueue
	{
	public:
		DirectX12RenderQueue(ID3D12Device1* device);
		DirectX12RenderQueue(const DirectX12RenderQueue&) = delete;
		DirectX12RenderQueue operator=(const DirectX12RenderQueue&) = delete;
		~DirectX12RenderQueue() = default;

		void ResetAlloctor() const;
		void ExecuteCommandLists(ID3D12GraphicsCommandList* graphicCommandList) const noexcept;

		void FlushCommandQueue();
		
		inline ID3D12CommandQueue* GetCommandQueueNative() { return m_CommandQueue.Get(); }
		inline ID3D12CommandAllocator* GetCommandAllocatorNative() { return m_CommandAllocator.Get(); }
	private:
		D3D12_COMMAND_QUEUE_DESC m_CommandQueueDesc;

		ComPtr<ID3D12Fence1> m_Fence;
		UINT64 m_CurrFence = 0;

		ComPtr<ID3D12CommandQueue> m_CommandQueue;
		ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
	};

}