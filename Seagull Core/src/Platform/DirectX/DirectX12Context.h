#pragma once

#include "Renderer/GraphicContext.h"
#include "DirectXHelper.h"

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

using Microsoft::WRL::ComPtr;

namespace SG
{

	class DirectX12Context : public GraphicContext
	{
	public:
		DirectX12Context() = delete;
		DirectX12Context(const DirectX12Context&) = delete;
		DirectX12Context operator=(const DirectX12Context& rhs) = delete;
		~DirectX12Context();

		static void Init(ID3D12Device1* device);

		static void Enable4xMSAA(bool enable) { m_4xMSAAState = enable; }
		inline static bool Get4xMSAAState() { return m_4xMSAAState; }
		inline static UINT Get4xMSAAQualityCount() { return m_4xMSAAQuality; }

		inline static DXGI_FORMAT GetBackBufferFormat() { return m_BackBufferFormat; }
		inline static DXGI_FORMAT GetDepthStencilFormat() { return m_DepthStencilFormat; }

		static uint32_t GetDescHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type);

		inline static UINT GetRtvDescriptorSize() { return m_RtvDescriptorSize; }
		inline static UINT GetDsvDescriptorSize() { return m_DsvDescriptorSize; }
		inline static UINT GetCbvDescriptorSize() { return m_CbvDescriptorSize; }

		static void LogOutAdaptersToConsole(IDXGIFactory7* factory, bool enableDisplayModeDetail = false);
	private:
		static void EnumAdapters(IDXGIFactory7* factory);
		static void LogAdapters(IDXGIAdapter* adapter, bool enableDisplayModeDetail);
		static void LogDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);
	private:
		static UINT m_RtvDescriptorSize, m_DsvDescriptorSize, m_CbvDescriptorSize;  // each descriptor size of the device
		static bool m_4xMSAAState;
		static UINT m_4xMSAAQuality;                                                // quality level of 4X MSAA
		static DXGI_FORMAT m_BackBufferFormat;
		static DXGI_FORMAT m_DepthStencilFormat;

		static std::vector<IDXGIAdapter*> m_AdaptersList;
	};

}