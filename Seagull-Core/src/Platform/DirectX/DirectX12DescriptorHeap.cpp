#include "sgpch.h"
#include "DirectX12DescriptorHeap.h"

#include "DirectX12Context.h"

namespace SG
{

	DirectX12DescriptorHeap::DirectX12DescriptorHeap(ID3D12Device1* device, 
		D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flag)
		: m_BindingType(BindingType::None)
	{
		m_HeapDesc.NumDescriptors = numDescriptors;
		m_HeapDesc.Type = type;
		m_HeapDesc.Flags = flag;
		m_HeapDesc.NodeMask = 0;

		ThrowIfFailed(device->CreateDescriptorHeap(&m_HeapDesc, IID_PPV_ARGS(m_DescHeap.GetAddressOf())));
		m_CPUHeapStart = m_DescHeap->GetCPUDescriptorHandleForHeapStart();
		m_GPUHeapStart = m_DescHeap->GetGPUDescriptorHandleForHeapStart();
		m_DescHandleIncrementSize = DirectX12Context::GetDescHandleIncrementSize(type);
	}

	void DirectX12DescriptorHeap::CreateRtv(ID3D12Device1* device, ID3D12Resource* resource, uint32_t index, const D3D12_RENDER_TARGET_VIEW_DESC* desc /*= nullptr*/)
	{
		if ( m_BindingType != BindingType::RTV && m_BindingType != BindingType::None)
			SG_CORE_ASSERT(false, "This Heap already have {0}", m_BindingType);
		m_BindingType = BindingType::RTV;
		device->CreateRenderTargetView(resource, desc, CPUOffset(index));
	}

	void DirectX12DescriptorHeap::CreateDsv(ID3D12Device1* device, ID3D12Resource* resource, uint32_t index, const D3D12_DEPTH_STENCIL_VIEW_DESC* desc)
	{
		if (m_BindingType != BindingType::DSV && m_BindingType != BindingType::None)
			SG_CORE_ASSERT(false, "This Heap already have {0}", m_BindingType);
		m_BindingType = BindingType::DSV;
		device->CreateDepthStencilView(resource, desc, CPUOffset(index));
	}

	void DirectX12DescriptorHeap::CreateSrv(ID3D12Device1* device, ID3D12Resource* resource, uint32_t index, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc)
	{
		if (m_BindingType != BindingType::SRV && m_BindingType != BindingType::None)
			SG_CORE_ASSERT(false, "This Heap already have {0}", m_BindingType);
		m_BindingType = BindingType::SRV;
		device->CreateShaderResourceView(resource, desc, CPUOffset(index));
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12DescriptorHeap::GetView(uint32_t index) const
	{
		if (index >= m_HeapDesc.NumDescriptors) index = m_HeapDesc.NumDescriptors - 1;
		D3D12_CPU_DESCRIPTOR_HANDLE handle = { m_CPUHeapStart.ptr + static_cast<uint64_t>(index) * m_DescHandleIncrementSize };
		return handle;
	}

	constexpr D3D12_CPU_DESCRIPTOR_HANDLE DirectX12DescriptorHeap::CPUOffset(uint32_t index) const
	{
		if (index >= m_HeapDesc.NumDescriptors) index = m_HeapDesc.NumDescriptors - 1;
		D3D12_CPU_DESCRIPTOR_HANDLE handle = { m_CPUHeapStart.ptr + static_cast<uint64_t>(index) * m_DescHandleIncrementSize };
		return handle;
	}

}