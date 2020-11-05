#include "sgpch.h"
#include "DirectX12Context.h"

#include "Core/Core.h"

namespace SG
{
	bool DirectX12Context::m_4xMSAAState = true;
	UINT DirectX12Context::m_4xMSAAQuality = 0;
	DXGI_FORMAT DirectX12Context::m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT DirectX12Context::m_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	UINT DirectX12Context::m_RtvDescriptorSize = 0;
	UINT DirectX12Context::m_DsvDescriptorSize = 0;
	UINT DirectX12Context::m_CbvDescriptorSize = 0;

	DirectX12Context::DirectX12Context(ComPtr<ID3D12Device> device)
		:m_D3dDevice(device)
	{
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
	}

	void DirectX12Context::SwapBuffers()
	{

	}

}