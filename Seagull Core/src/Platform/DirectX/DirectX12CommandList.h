#pragma once

#include <wrl.h>
#include <d3d12.h>

using Microsoft::WRL::ComPtr;

namespace SG
{
	class DirectX12RenderQueue;

	class DirectX12CommandList
	{
	public:
		DirectX12CommandList(ID3D12Device1* device, const Ref<DirectX12RenderQueue>& renderQueue);
		DirectX12CommandList(const DirectX12CommandList&) = delete;
		DirectX12CommandList operator=(const DirectX12CommandList&) = delete;
		~DirectX12CommandList() = default;

		void ResourceBarrier(UINT numBarriers, ID3D12Resource* resource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter) const noexcept;

		void SetViewports(UINT num, const D3D12_VIEWPORT* viewport) const noexcept;
		void SetScissorRect(UINT num, const D3D12_RECT* rect) const noexcept;

		void ClearRtv(D3D12_CPU_DESCRIPTOR_HANDLE Rtv, const FLOAT* color, UINT numRects, const D3D12_RECT* pRects) const noexcept;
		void ClearDsv(D3D12_CPU_DESCRIPTOR_HANDLE Dsv, D3D12_CLEAR_FLAGS clearFlags, FLOAT depth, UINT8 stencil, UINT numRects, const D3D12_RECT* pRects) const noexcept;
		void SetRenderTarget(UINT numRenderTargetDesc,
			const D3D12_CPU_DESCRIPTOR_HANDLE* Rtv, 
			bool RTsSingleHandleToDescriptorRange,
			const D3D12_CPU_DESCRIPTOR_HANDLE* Dsv) const noexcept;

		void Reset(ID3D12CommandAllocator* commandAllocator, ID3D12PipelineState* pipelineState) const;
		void Close() const noexcept;

		ID3D12GraphicsCommandList* GetCommandListNative() { return m_CommandList.Get(); }
	private:
		ComPtr<ID3D12GraphicsCommandList> m_CommandList;
	};

}