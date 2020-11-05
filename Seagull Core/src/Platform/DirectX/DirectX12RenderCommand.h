#pragma once

#include <wrl.h>
#include <d3d12.h>

using Microsoft::WRL::ComPtr;

namespace SG
{
	// Because command queue is a free-thread object,
	// each thread should have their own command list and command allocator
	class DirectX12RenderCommand
	{
	public:
		DirectX12RenderCommand(ComPtr<ID3D12Device> device);
		~DirectX12RenderCommand() = default;

		void Submit();
		void SetViewport();
		void SetScissorRectangle();

		ComPtr<ID3D12CommandQueue> GetCommandQueue() const { return m_CommandQueue; }
		ComPtr<ID3D12GraphicsCommandList> GetCommandList() const { return m_CommandList; }
	private:
		ComPtr<ID3D12Device> m_D3dDevice;

		ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
		ComPtr<ID3D12CommandQueue> m_CommandQueue;
		ComPtr<ID3D12GraphicsCommandList> m_CommandList;
	};

}