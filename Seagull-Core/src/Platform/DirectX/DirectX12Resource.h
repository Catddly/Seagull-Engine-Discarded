#pragma once

#include <wrl.h>
#include <d3d12.h>

using Microsoft::WRL::ComPtr;

namespace SG
{

	class DirectX12Resource
	{
	public:
		DirectX12Resource() = default;
		DirectX12Resource(const DirectX12Resource&) = default;
		~DirectX12Resource() = default;

		ID3D12Resource* GetNativePtr() { return m_Resource.Get(); }
		ComPtr<ID3D12Resource>& GetPtr() { return m_Resource; }

		void Reset() noexcept;
	private:
		ComPtr<ID3D12Resource> m_Resource;
	};

}