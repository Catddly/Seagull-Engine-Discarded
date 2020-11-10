#include "sgpch.h"
#include "DirectX12DescriptorHeap.h"

#include "DirectX12Context.h"

namespace SG
{

	DirectX12DescriptorHeap::DirectX12DescriptorHeap(ID3D12Device1* device, 
		D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors) : m_BindingType(BindingType::None)
	{
		m_HeapDesc.NumDescriptors = numDescriptors;
		m_HeapDesc.Type = type;
		m_HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE; // TODO: support D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
		m_HeapDesc.NodeMask = 0;

		ThrowIfFailed(device->CreateDescriptorHeap(&m_HeapDesc, IID_PPV_ARGS(m_DescHeap.GetAddressOf())));
		m_CPUHeapStart = m_DescHeap->GetCPUDescriptorHandleForHeapStart();
		m_GPUHeapStart = m_DescHeap->GetGPUDescriptorHandleForHeapStart();
		m_DescHandleIncrementSize = DirectX12Context::GetDescHandleIncrementSize(type);
	}

	void DirectX12DescriptorHeap::CreateRtv(ID3D12Device1* device, ID3D12Resource* resource, uint32_t index, const D3D12_RENDER_TARGET_VIEW_DESC* desc /*= nullptr*/)
	{
		m_BindingType = BindingType::RTV;
		device->CreateRenderTargetView(resource, desc, Offset(index));
	}

	void DirectX12DescriptorHeap::CreateDsv(ID3D12Device1* device, ID3D12Resource* resource, uint32_t index, const D3D12_DEPTH_STENCIL_VIEW_DESC* desc)
	{
		m_BindingType = BindingType::DSV;
		device->CreateDepthStencilView(resource, desc, Offset(index));
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12DescriptorHeap::GetView(uint32_t index) const
	{
		if (index >= m_HeapDesc.NumDescriptors) index = m_HeapDesc.NumDescriptors - 1;
		D3D12_CPU_DESCRIPTOR_HANDLE handle = { m_CPUHeapStart.ptr + static_cast<uint64_t>(index) * m_DescHandleIncrementSize };
		return handle;
	}

	constexpr D3D12_CPU_DESCRIPTOR_HANDLE DirectX12DescriptorHeap::Offset(uint32_t index) const
	{
		if (index >= m_HeapDesc.NumDescriptors) index = m_HeapDesc.NumDescriptors - 1;
		D3D12_CPU_DESCRIPTOR_HANDLE handle = { m_CPUHeapStart.ptr + static_cast<uint64_t>(index) * m_DescHandleIncrementSize };
		return handle;
	}

}