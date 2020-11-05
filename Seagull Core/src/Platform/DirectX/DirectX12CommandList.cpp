#include "sgpch.h"
#include "DirectX12CommandList.h"

#include "DirectX12RenderQueue.h"
#include "DirectXHelper.h"
#include "d3dx12.h"

namespace SG
{
	DirectX12CommandList::DirectX12CommandList(ComPtr<ID3D12Device1> device)
		:m_D3dDevice(device)
	{}

	void DirectX12CommandList::Init(const Ref<DirectX12RenderQueue>& rq)
	{
		ThrowIfFailed(m_D3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, rq->GetCommandAllocatorNative(),
			nullptr /* PSO */, IID_PPV_ARGS(m_CommandList.GetAddressOf())));
	}

	void DirectX12CommandList::ResourceBarrier(UINT numBarriers, ID3D12Resource* resource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter) const noexcept
	{
		m_CommandList->ResourceBarrier(numBarriers, 
			&CD3DX12_RESOURCE_BARRIER::Transition(resource, stateBefore, stateAfter));
	}

	void DirectX12CommandList::SetViewports(UINT num, const D3D12_VIEWPORT* viewport) const noexcept
	{
		m_CommandList->RSSetViewports(num, viewport);
	}

	void DirectX12CommandList::SetScissorRect(UINT num, const D3D12_RECT* rect) const noexcept
	{
		m_CommandList->RSSetScissorRects(num, rect);
	}

	void DirectX12CommandList::ClearRtv(D3D12_CPU_DESCRIPTOR_HANDLE Rtv, const FLOAT* color, UINT numRects, const D3D12_RECT* pRects) const noexcept
	{
		m_CommandList->ClearRenderTargetView(Rtv, color, numRects, pRects);
	}

	void DirectX12CommandList::ClearDsv(D3D12_CPU_DESCRIPTOR_HANDLE Dsv, D3D12_CLEAR_FLAGS clearFlags, FLOAT depth, UINT8 stencil, UINT numRects, const D3D12_RECT* pRects) const noexcept
	{
		m_CommandList->ClearDepthStencilView(Dsv, clearFlags, depth, stencil, numRects, pRects);
	}

	void DirectX12CommandList::SetRenderTarget(UINT numRenderTargetDesc, const D3D12_CPU_DESCRIPTOR_HANDLE* Rtv, bool RTsSingleHandleToDescriptorRange, const D3D12_CPU_DESCRIPTOR_HANDLE* Dsv) const noexcept
	{
		m_CommandList->OMSetRenderTargets(numRenderTargetDesc, Rtv, RTsSingleHandleToDescriptorRange, Dsv);
	}

	void DirectX12CommandList::Reset(ID3D12CommandAllocator* commandAllocator, ID3D12PipelineState* pipelineState) const
	{
		ThrowIfFailed(m_CommandList->Reset(commandAllocator, pipelineState));
	}

	void DirectX12CommandList::Close() const noexcept
	{
		m_CommandList->Close();
	}

}