#pragma once

#include "Renderer/GraphicContext.h"
#include "DirectXHelper.h"

#include <wrl.h>
#include <d3d12.h>

using Microsoft::WRL::ComPtr;

namespace SG
{

	class DirectX12Context : public GraphicContext
	{
	public:
		DirectX12Context(ComPtr<ID3D12Device> device);
		DirectX12Context(const DirectX12Context&) = delete;
		DirectX12Context operator=(const DirectX12Context& rhs) = delete;
		~DirectX12Context() = default;

		virtual void Init() override;

		void Enable4xMSAA(bool enable) { m_4xMSAAState = enable; }
		inline static bool Get4xMSAAState() { return m_4xMSAAState; }
		inline static UINT Get4xMSAAQualityCount() { return m_4xMSAAQuality; }

		inline static DXGI_FORMAT GetBackBufferFormat() { return m_BackBufferFormat; }
		inline static DXGI_FORMAT GetDepthStencilFormat() { return m_DepthStencilFormat; }

		inline static UINT GetRtvDescriptorSize() { return m_RtvDescriptorSize; }
		inline static UINT GetDsvDescriptorSize() { return m_DsvDescriptorSize; }
		inline static UINT GetCbvDescriptorSize() { return m_CbvDescriptorSize; }
	private:
		ComPtr<ID3D12Device> m_D3dDevice;

		static UINT m_RtvDescriptorSize, m_DsvDescriptorSize, m_CbvDescriptorSize;  // each descriptor size of the device
		static bool m_4xMSAAState;
		static UINT m_4xMSAAQuality;                                                // quality level of 4X MSAA
		static DXGI_FORMAT m_BackBufferFormat;
		static DXGI_FORMAT m_DepthStencilFormat;
	};

}