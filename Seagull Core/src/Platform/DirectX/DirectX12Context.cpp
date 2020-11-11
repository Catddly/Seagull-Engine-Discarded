#include "sgpch.h"
#include "DirectX12Context.h"

#include "Core/Core.h"

namespace SG
{

	bool DirectX12Context::m_4xMSAAState = false;
	UINT DirectX12Context::m_4xMSAAQuality = 0;
	DXGI_FORMAT DirectX12Context::m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT DirectX12Context::m_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	UINT DirectX12Context::m_RtvDescriptorSize = 0;
	UINT DirectX12Context::m_DsvDescriptorSize = 0;
	UINT DirectX12Context::m_CbvDescriptorSize = 0;

	std::vector<IDXGIAdapter*> DirectX12Context::m_AdaptersList;

	DirectX12Context::~DirectX12Context()
	{
		for (size_t i = 0; i < m_AdaptersList.size(); ++i)
			ReleaseCom(m_AdaptersList[i]);
	}

	void DirectX12Context::Init(ID3D12Device1* device)
	{
		// Get the descriptor size of the device
		m_RtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		m_DsvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		m_CbvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		// Check the support of 4X MSAA
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS sampleQualityLevels;
		sampleQualityLevels.Format = m_BackBufferFormat;
		sampleQualityLevels.SampleCount = 4;
		sampleQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		sampleQualityLevels.NumQualityLevels = 0;
		ThrowIfFailed(device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
			&sampleQualityLevels,
			sizeof(sampleQualityLevels)));

		m_4xMSAAQuality = sampleQualityLevels.NumQualityLevels;
		SG_CORE_ASSERT(m_4xMSAAQuality > 0, "Unexcepted MSAA quality level!");
	}

	uint32_t DirectX12Context::GetDescHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type)
	{
		switch (type)
		{
			case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
				return m_CbvDescriptorSize; break;
			case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
				SG_CORE_ASSERT("Unknown D3D12_DESCRIPTOR_HEAP_TYPE!"); break;
			case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
				return m_RtvDescriptorSize; break;
			case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
				return m_DsvDescriptorSize; break;
			case D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES:
				SG_CORE_ASSERT("Unknown D3D12_DESCRIPTOR_HEAP_TYPE!"); break;
		}

		SG_CORE_ASSERT("Unknown D3D12_DESCRIPTOR_HEAP_TYPE!");
	}

	void DirectX12Context::LogOutAdaptersToConsole(IDXGIFactory7* factory, bool enableDisplayModeDetail)
	{
		EnumAdapters(factory);
		for (size_t i = 0; i < m_AdaptersList.size(); ++i)
		{
			LogAdapters(m_AdaptersList[i], enableDisplayModeDetail);
		}
	}

	void DirectX12Context::EnumAdapters(IDXGIFactory7* factory)
	{
		UINT i = 0;
		IDXGIAdapter* adapter = nullptr;
		while (factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_ADAPTER_DESC desc;
			adapter->GetDesc(&desc);

			std::wstring text = L"***Adapter: ";
			text += desc.Description;
			SG_CORE_INFO(text);

			m_AdaptersList.push_back(adapter);
			++i;
		}
	}

	void DirectX12Context::LogAdapters(IDXGIAdapter* adapter, bool enableDisplayModeDetail)
	{
		UINT i = 0;
		IDXGIOutput* output = nullptr;
		while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_OUTPUT_DESC desc;
			output->GetDesc(&desc);

			std::wstring text = L"***Output: ";
			text += desc.DeviceName;
			SG_CORE_INFO(text);

			if (enableDisplayModeDetail)
				LogDisplayModes(output, m_BackBufferFormat);

			ReleaseCom(output);
			++i;
		}
	}

	void DirectX12Context::LogDisplayModes(IDXGIOutput* output, DXGI_FORMAT format)
	{
		UINT count = 0;
		UINT flags = 0;

		// Call with nullptr to get list count.
		output->GetDisplayModeList(format, flags, &count, nullptr);

		std::vector<DXGI_MODE_DESC> modeList(count);
		output->GetDisplayModeList(format, flags, &count, &modeList[0]);

		for (auto& x : modeList)
		{
			UINT n = x.RefreshRate.Numerator;
			UINT d = x.RefreshRate.Denominator;
			std::wstring text =
				L"  Width = " + std::to_wstring(x.Width) +
				L"  Height = " + std::to_wstring(x.Height) +
				L"  Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d);

			SG_CORE_INFO(text);
		}
	}

}